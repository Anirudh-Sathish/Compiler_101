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
		void inspectGEP(Module &M, map<string,int>* fieldCountMap,GetElementPtrInst *GEP)
		{
			Type *source_type = GEP->getSourceElementType();
            Value *ptr_operand = GEP->getPointerOperand();
			if(GetElementPtrInst *GEPInst = dyn_cast<GetElementPtrInst>(ptr_operand))
			{
				inspectGEP(M,fieldCountMap,GEPInst);
			}
			if (StructType *structType = dyn_cast<StructType>(source_type))
			{
				APInt ap_offset(64, 0, false);
                GEP->accumulateConstantOffset(M.getDataLayout(), ap_offset);
                int64_t offset_result = ap_offset.getSExtValue()/4;
                string fieldName = structType->getName().str() +to_string(offset_result);
                (*(fieldCountMap))[fieldName] +=1;
			}
			
		}
		void incrementFieldCount(Module &M,BasicBlock &BB, map<string,int>* fieldCountMap)
		{
			for(auto &I: BB)
			{
				if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(&I))
				{
					errs()<<I<<"\n";
					inspectGEP(M,fieldCountMap,GEP);
					errs()<<"\n";
				}
				
			}
		}
		void processFunction(Module &M, Function &F, map<string,int>* fieldCountMap)
		{
			for(auto &BB: F)
			{
				incrementFieldCount(M,BB,fieldCountMap);
			}
		}
	
		PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
		{			
			LLVMContext &context = M.getContext();
			map<string,int>fieldCountMap; 
			collectStructFieldCounts(M,&fieldCountMap);
			// getFieldCount(M,&fieldCountMap);
			for(auto &F:M)
			{
				processFunction(M,F,&fieldCountMap);
			}
			for(auto field: fieldCountMap)
			{
				errs()<<"Field: "<<field.first<<"Count: "<<field.second<<"\n";
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
