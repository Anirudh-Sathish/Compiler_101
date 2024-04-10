/*Basic Hello World Pass to get information*/
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CFG.h"
#include<iostream>


using namespace llvm;


namespace
{

  void instruction_counter(Function &F)
  {
    int count = 0;
    for(Instruction &i : instructions(F))
    {
      count++;
    }
    errs() <<"Fuction name: "<<F.getName()<<" Instructions: "<<count <<"\n";
  }
  void max_ins_block(Function &F)
  {
	int max = 0;
	BasicBlock *max_block = nullptr;
	for (BasicBlock &BB : F)
	{
		int inst_count = 0; 
		inst_count = BB.size();
		if(inst_count>max)
		{
			max = inst_count;
			max_block = &BB;
		}
	}
    if (max_block) {
        errs() << "Block with Maximum Instructions: " << max_block->getName()
               << " (" << max << " instructions)\n";
    }
  }
  void block_counter(Function &F)
  {
	errs()<<"Blocks in "<<F.getName()<<" : "<<F.size()<<"\n";
  }
  void max_predecessors(Function &F)
  {
	int max = 0;
	BasicBlock *max_block = nullptr;
	for (BasicBlock &BB : F)
	{
		int pred_count = 0;
		for (BasicBlock *Pred : predecessors(&BB))pred_count++;
		if(pred_count >max)
		{
			max = pred_count;
			max_block = &BB;
		}
	}
	if (max_block) {
        errs() << "Block with Maximum Predecessors: " << max_block->getName()
               << " (" << max << " Predecessors)\n";
    }
  }
  void max_successors(Function &F)
  {
	int max = 0;
	BasicBlock *max_block = nullptr;
	for (BasicBlock &BB : F)
	{
		int successors_count = 0;
		for (BasicBlock *Successor : successors(&BB))successors_count++;
		if(successors_count >max)
		{
			max = successors_count;
			max_block = &BB;
		}
	}
	if (max_block) {
        errs() << "Block with Maximum successors: " << max_block->getName()
               << " (" << max << " successors)\n";
    }
  }

  struct HelloWorld : PassInfoMixin<HelloWorld>
  {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &)
    {
      instruction_counter(F);
	  block_counter(F);
	  max_ins_block(F);
	  max_predecessors(F);
	  max_successors(F);
	  errs()<<"\n";
      return PreservedAnalyses::all();
    }
    static bool isRequired() { return true; }
  };
  

} 


llvm::PassPluginLibraryInfo getHelloWorldPluginInfo()
{
  	return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING,[](PassBuilder &PB)
	{
        PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
				{
					if (Name == "hello-world")
					{
						FPM.addPass(HelloWorld());
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
  return getHelloWorldPluginInfo();
}