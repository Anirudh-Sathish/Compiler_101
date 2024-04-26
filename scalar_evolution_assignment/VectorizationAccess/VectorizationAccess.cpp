/*VectorizationAccess Implementation*/
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Transforms/Scalar.h"
#include<iostream>
#include<vector>
#include<map>
#include <utility>
using namespace std;

using namespace llvm;


namespace
{

	struct MyPass : public PassInfoMixin<MyPass>
	{
		bool vectorizable_compatible(ScalarEvolution &SE,Function &F)
		{
			APInt previous;
			int cnt = 0;
			for(BasicBlock &BB:F)
			{
				for(Instruction &I: BB)
				{
					if(StoreInst *SI = dyn_cast<StoreInst>(&I))
					{
						Value *pointer_operand = SI->getPointerOperand();
						const SCEV *pointer_scev = SE.getSCEV(pointer_operand);
						APInt min_access = SE.getUnsignedRangeMin(pointer_scev);
						if(cnt ==0)previous=min_access;
						else
						{
							uint64_t previous_value = previous.getZExtValue();
                			uint64_t min_access_value = min_access.getZExtValue();
							if(previous_value >min_access_value)return false;
						}
						cnt++;					
					}
				}
			}
			return true;
		}
		void reorder_vectorizable_access(ScalarEvolution &SE, Function &F)
		{
			// Reorder array access to make loops vectorizable
			vector<pair<StoreInst*,uint64_t>>pairs;
			APInt previous;
			for(BasicBlock &BB:F)
			{
				for(Instruction &I: BB)
				{
					if(StoreInst *SI = dyn_cast<StoreInst>(&I))
					{
						Value *pointer_operand = SI->getPointerOperand();
						const SCEV *pointer_scev = SE.getSCEV(pointer_operand);
						APInt min_access = SE.getUnsignedRangeMin(pointer_scev);
						uint64_t min_access_value = min_access.getZExtValue();
						pairs.push_back({SI,min_access_value});
					}
				}
			}
			std::sort(pairs.begin(), pairs.end(),
			[](const std::pair<StoreInst*, uint64_t> &a,
			const std::pair<StoreInst*, uint64_t> &b)
			{
                return a.second < b.second;
            });
			reorder_stores(F,pairs);
		}

		void reorder_stores(Function &F,vector<pair<StoreInst*,uint64_t>>pairs)
		{
			// Reorders the store instructions according to the order
			// they are given in 
			Instruction *si;
			if(pairs.size()>0) si = dyn_cast<Instruction>(pairs[0].first);
			BasicBlock *BB =(*(si)).getParent();
			int store_count = pairs.size();
			int cnt = 0;
			for(auto &I: *(BB))
			{
				if(cnt ==store_count)
				{
					Instruction* new_ins;
					while(cnt != 0)
					{
						new_ins = dyn_cast<Instruction>(pairs[(store_count-cnt)].first);
						(*(new_ins)).moveBefore(&I);
						cnt--;
					}
				}
				if(StoreInst *SI = dyn_cast<StoreInst>(&I))cnt++;				
			}
			errs()<<"\nRearranged access to enable vectorization \n\n";
		}
		PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM)
		{
			ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
			if(F.getName() == "vectorize")
			{
				bool possible = vectorizable_compatible(SE,F);
				if(possible)errs()<<"This function is vectorizable"<<"\n";
				else
				{
					errs()<<"This function is not vectorizable"<<"\n";
					reorder_vectorizable_access(SE,F);
				}
				
			}
			return PreservedAnalyses::none();
		}
	};
 

} 

llvm::PassPluginLibraryInfo VectorizationAccessPluginInfo()
{
  	return {LLVM_PLUGIN_API_VERSION, "VectorizationAccess", LLVM_VERSION_STRING,[](PassBuilder &PB)
	{
        PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
				{
					if(Name == "my-pass")
					{
						FPM.addPass(MyPass());
						return true;
					}
					return false;
                }
				);
          	}
	};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return VectorizationAccessPluginInfo();
}