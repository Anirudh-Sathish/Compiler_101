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
#include<iostream>
#include<vector>
#include<map>
using namespace std;

using namespace llvm;


namespace
{

	struct MyPass : public PassInfoMixin<MyPass>
	{
		
		PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
		{
			LLVMContext &context = M.getContext();
			for(auto &F: M)
			{
				for(auto &BB: F)
				{
					for(auto &I: BB)
					{
						if(auto *GEPInst = dynamic_cast<GetElementPtrInst>(&I))
						{
							if(GEPInst->getNumIndices>1)
							{
								errs()<<"Is a struct \n";
							}
						}
					}
				}
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