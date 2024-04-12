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
  
	struct MyPass : public PassInfoMixin<MyPass>
	{
	void view_global_variables(Module &M)
	{
		for(auto global = M.global_begin(); global !=  M.global_end();global++)
		{
			GlobalVariable *GV = &(*global);
			errs()<<GV->getName()<<"\n";
		}
	}
  	void add_global_variable(Module &M,std::string name,Type *ty, Constant *value)
	{
        GlobalVariable *globalVar = new GlobalVariable(
        M, ty, false, GlobalValue::ExternalLinkage,
        value, name);
    }
	void first_g_variation(Module &M,Type *intType )
	{
		int count;
		for(auto &F: M)
		{
			GlobalVariable *global = M.getNamedGlobal("G");
			count = 0;
			for(auto &BB: F)
			{
				Instruction &firstInst = *BB.getFirstNonPHI();
				IRBuilder<> builder(&firstInst); 
                Value *oneValue = ConstantInt::get(intType, count);
				builder.CreateStore(oneValue, global);
				count++;
			}
		}
	}
	void g_xor(Module &M,Type *intType)
	{
		int count;
		int flag =0;
		Value *countValue;
		Value *intValueValue;
		for(auto &F:M)
		{
			count= 0;
			GlobalVariable *global = M.getNamedGlobal("G");
			for(auto &BB: F)
			{
				if(BB.getName() == "entry")
				{
					count++;
					continue;
				}
				for(auto &I: BB)
				{
					Value *previousOperand = nullptr;
					if(flag ==1)
					{
						IRBuilder<> builder(&I);
						Value* xorResult = builder.CreateXor(countValue, intValueValue, "xorResult");
						builder.CreateStore(xorResult, global);
						flag = 0;
					}
					for (Use &U : I.operands())
					{
						Value *V = U.get();
						if(V == global)
						{
							if (previousOperand)
							{
								auto *constInt = dyn_cast<ConstantInt>(previousOperand);
								int intValue = constInt->getSExtValue(); 
								countValue = ConstantInt::get(intType, count);
								intValueValue = ConstantInt::get(intType, intValue);								
								flag = 1;
							}
						}
						previousOperand = V;
               		}
				}
				count++;
			}
		}
	}
  	PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
  	{
		LLVMContext &context = M.getContext();
  		Type *intType = Type::getInt32Ty(context);
    	Constant *initValue = ConstantInt::get(intType, 0);
    	add_global_variable(M,"G",intType,initValue);
		view_global_variables(M);
		first_g_variation(M,intType);
		
		errs()<<"\n\n\n\n\n\n";
		g_xor(M,intType);

    	return PreservedAnalyses::none();
  	}
	};
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

llvm::PassPluginLibraryInfo updatedHelloWorldPluginInfo()
{
  	return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING,[](PassBuilder &PB)
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
  return updatedHelloWorldPluginInfo();
}