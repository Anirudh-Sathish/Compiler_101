/*Module to generate illegal branch*/
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

	struct IllegalBranchPass : public PassInfoMixin<IllegalBranchPass>
	{
		BasicBlock* get_new_edge(BasicBlock &BB)
		{
			/*Finds a block where a new edge can be introduced from this block*/
			for (BasicBlock *Succ : successors(&BB))
			{
				for (BasicBlock *gk : successors(Succ))
				{
					bool found = false;
					for (BasicBlock *Succ_p : successors(&BB))
					{
						if (Succ_p == gk) 
						{
							found = true;
							break;
						}
					}
					if (!found) {
						return gk;
					}
				}
			}
			return nullptr;
		}

		void induce_illegal_branch(Module &M, LLVMContext &Context) {
			int flag = 0;
            for (auto &F : M)
			{
				for (auto &BB : F)
				{
					if(BB.getName() =="entry")continue;
					
					Instruction* TermInst = BB.getTerminator();
					if (!TermInst)
                    {
						continue; 
					}
					BasicBlock *res = get_new_edge(BB);
					if (res)
                    {
						IRBuilder<> builder(TermInst);
						BranchInst* NewBranchInst = builder.CreateBr(res);
						TermInst->replaceAllUsesWith(NewBranchInst);
						TermInst->eraseFromParent();
                        flag =1;
						break;
					}
				}
			}
            if(flag ==1)errs()<<"Created illegal branch \n";
			else errs()<<"Could not illegal branch \n";
		}
		PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
		{
			LLVMContext &context = M.getContext();
			induce_illegal_branch(M,context);
			return PreservedAnalyses::none();
		}
	};
 

} 

llvm::PassPluginLibraryInfo IllegalBranchPluginInfo()
{
  	return {LLVM_PLUGIN_API_VERSION, "illegal_generator", LLVM_VERSION_STRING,[](PassBuilder &PB)
	{
        PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>)
				{
					if(Name == "illegal-branch-pass")
					{
						MPM.addPass(IllegalBranchPass());
						return true;
					}
					return false;
                }
				);
          	}
	};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
  return IllegalBranchPluginInfo();
}