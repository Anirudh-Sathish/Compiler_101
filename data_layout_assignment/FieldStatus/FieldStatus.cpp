/*FieldStatus Implementation*/
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Analysis/LoopInfo.h"
#include<iostream>
#include<vector>
#include<map>
#include <unordered_map>
using namespace std;
using namespace llvm;


namespace
{

	struct MyPass : public PassInfoMixin<MyPass>
	{
		std::unordered_map<StructType*, unsigned> StructFieldCounts;
		void collectStructFieldCounts(Module &M, map<string,int>* fieldCountMap)
		{
			// initalises the count of sturct fields
			for(Type *type: M.getIdentifiedStructTypes())
			{
				if (StructType *structType = dyn_cast<StructType>(type))
				{
					for (unsigned i = 0; i < structType->getNumElements(); ++i)
					{
						Type *fieldType = structType->getElementType(i);
						string fieldName = structType->getName().str()+ std::to_string(i);
						// Map the field name to 0
						(*(fieldCountMap))[fieldName] = 0;
                	}
					
				}
			}
    	}
		void incrementFieldCount(Module &M,BasicBlock &BB, map<string,int>* fieldCountMap, unsigned count)
		{
			// increments the field count of Instructions in Basic Block
			for(auto &I: BB)
			{
				if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I))
				{
					inspectGEP(M,fieldCountMap,GEP,count);
				}
			}
		}
		void inspectGEP(Module &M, map<string,int>* fieldCountMap,GetElementPtrInst *GEP, unsigned count)
		{
			// inspects GEP, and identifies struct type GEP, incrementing the field
			// count of struct
			Type *source_type = GEP->getSourceElementType();
            Value *ptr_operand = GEP->getPointerOperand();
			if(GetElementPtrInst *GEPInst = dyn_cast<GetElementPtrInst>(ptr_operand))
			{
				inspectGEP(M,fieldCountMap,GEPInst,count);
			}
			if(AllocaInst *AllocI = dyn_cast<AllocaInst>(ptr_operand))
			{
				Type *allocated_type = AllocI->getAllocatedType();
				if (StructType *structType = dyn_cast<StructType>(allocated_type))
				{
					APInt ap_offset(64, 0, false);
					GEP->accumulateConstantOffset(M.getDataLayout(), ap_offset);
					int64_t offset_result = ap_offset.getSExtValue()/4;
					string fieldName = structType->getName().str() +to_string(offset_result);
					(*(fieldCountMap))[fieldName] +=count;
				}
			}
		}
		unsigned computeLoopIterations(Loop *L, FunctionAnalysisManager &FAM, Function &F)
		{
			// computes the number of iterations of a given loop
			ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
			unsigned iterations = 1;
			BasicBlock *exitingBlock = L->getExitingBlock();
			unsigned TripCount = SE.getSmallConstantTripCount(L,exitingBlock);
			if(TripCount)iterations = TripCount;
			return iterations;
		}
		void processFunction(Module &M, Function &F, map<string,int>* fieldCountMap, FunctionAnalysisManager &FAM)
		{
			// identifies loops and increments the struct fields
			LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
			unsigned iterations;
			for(Loop *L:LI)
			{
				iterations = computeLoopIterations(L,FAM,F);
				for (BasicBlock *BB : L->getBlocks())
				{
					for(auto &I: *(BB))incrementFieldCount(M,*(BB),fieldCountMap,iterations);
				}
			}
			for (BasicBlock &BB : F)
			{
				iterations = 1;
				if (!LI.getLoopFor(&BB))incrementFieldCount(M, BB, fieldCountMap,iterations);
			}
		}
	
		PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
		{			
			LLVMContext &context = M.getContext();
			map<string,int>fieldCountMap; 
			collectStructFieldCounts(M,&fieldCountMap);
			FunctionAnalysisManager &FAM = MAM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
			for(auto &F: M)
			{
				if(F.isDeclaration())continue;
				processFunction(M,F,&fieldCountMap,FAM);
			}
			for(auto field: fieldCountMap)
			{
				errs()<<"Field: "<<field.first<<"Count: "<<field.second<<"\n";
				if(field.second == 0)errs()<<"Dead Field \n";
				else if(field.second <= 5) errs()<<"Cold Field \n";
				else errs()<<"Hot Field \n";	
			}
			return PreservedAnalyses::none();
		}
	};
 

} 

llvm::PassPluginLibraryInfo FieldStatusPluginInfo()
{
  	return {LLVM_PLUGIN_API_VERSION, "FieldStatus", LLVM_VERSION_STRING,[](PassBuilder &PB)
	{
        PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>)
				{
					if(Name == "my-pass")
					{
						MPM.addPass(MyPass());
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
  return FieldStatusPluginInfo();
}
