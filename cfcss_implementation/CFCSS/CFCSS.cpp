/*CFCSS Implementation*/
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
		void insert_printf(Module &M, LLVMContext &context)
		{
			FunctionType *printf_func_type = FunctionType::get(
        	IntegerType::getInt32Ty(context),
        	PointerType::getInt8Ty(context),
        	true);
			FunctionCallee printf_unc = M.getOrInsertFunction("printf", printf_func_type);
		}
		map<BasicBlock*,int> calculate_signature(Module &M)
		{
			map<BasicBlock*,int> sig;
			int count = 1;
			for(auto &F :M)
			{
				for(auto &BB:F)
				{
					sig[&BB] = count;
					count++;
				}
			}
			return sig;
		}
		Function *createErrorFunction(Module &M) {
			LLVMContext &context = M.getContext();
			Type *voidType = Type::getVoidTy(context);
			FunctionType *funcType = FunctionType::get(voidType, false);
			return Function::Create(funcType, GlobalValue::LinkageTypes::ExternalLinkage, "errorFunction", &M);
		}
		void add_global_variable(Module &M,std::string name,Type *ty, Constant *value)
		{
			GlobalVariable *globalVar = new GlobalVariable(
			M, ty, false, GlobalValue::ExternalLinkage,
			value, name);
		}
		void basic_instruct_cfcss(Module &M)
		{
			LLVMContext &context = M.getContext();
			Type *intType = Type::getInt32Ty(context);
			Constant *initValue = ConstantInt::get(intType, 0);
			add_global_variable(M,"G",intType,initValue);
			map<int,int> signature_d;
			int count = 1;
			for(auto &F :M)
			{
				errs()<<F.getName()<<"\n";
				if(F.getName() == "error_checker")continue;
				for(auto &BB:F)
				{
					GlobalVariable *global = M.getNamedGlobal("G");
					int pred_count = 0;
					for (BasicBlock *Pred : predecessors(&BB))pred_count++;
					if(pred_count ==0){
						Instruction &firstInst = *BB.getFirstNonPHI();
						IRBuilder<> builder(&firstInst); 
						Value *g_val = ConstantInt::get(intType, count);
						builder.CreateStore(g_val, global);
					}
					else if (pred_count ==1)
					{
						int d = count ^(count-1);
						Instruction &firstInst = *BB.getFirstNonPHI();
						IRBuilder<> builder(&firstInst); 
						Value *currentGValue = builder.CreateLoad(intType,global);
						Value *newGValue = builder.CreateXor(currentGValue, ConstantInt::get(intType, d));
						builder.CreateStore(newGValue, global);

						// Call a function to check values of newGValue and count
						Value *block_sign = ConstantInt::get(intType, count);
						std::vector<Value*> args = {newGValue, block_sign};
						builder.CreateCall(M.getFunction("error_checker"), args);
					}
					count++;
				}
			}
		}
		void insert_after_call(BasicBlock *Pred, int value,GlobalVariable *global,LLVMContext &context)
		{
			Type *intType = Type::getInt32Ty(context);
			int flag = 0;
			for(auto &I: *Pred)
			{
				if(flag == 1)
				{
					IRBuilder<> builder(&I); 
					Value *d_val = ConstantInt::get(intType, value);
					builder.CreateStore(d_val, global);
					flag = 0;
					break;
				}
				if (isa<CallInst>(&I)) flag =1;
			}

		}
		int addD(BasicBlock &BB,GlobalVariable *global,LLVMContext &context, map<BasicBlock*,int> sig)
		{
			int itr =0;
			int s_prev;
			for(BasicBlock *Pred : predecessors(&BB))
			{
				int blocksgn = sig[Pred];
				if(itr ==0)
				{
					s_prev = blocksgn;
					insert_after_call(Pred,0,global,context);
				}
				else
				{
					int D = s_prev ^blocksgn;
					insert_after_call(Pred,D,global,context);
				}
				itr++;
			}
			return s_prev;	
		}
		void fan_instruct_cfcss(Module &M, map<BasicBlock*,int> sig)
		{
			// insert D as global variable 
			LLVMContext &context = M.getContext();
			Type *intType = Type::getInt32Ty(context);
			Constant *initValue = ConstantInt::get(intType, 0);
			add_global_variable(M,"D",intType,initValue);
			int count = 1;
			for(auto &F :M)
			{
				if(F.getName() == "error_checker")continue;
				for(auto &BB:F)
				{
					GlobalVariable *global_d = M.getNamedGlobal("D");
					GlobalVariable *global_g = M.getNamedGlobal("G");
					int pred_count = 0;
					// find predecessor count
					for (BasicBlock *Pred : predecessors(&BB))pred_count++;
					int s_prev;
					// if more than or equal to 2 pred, then fan
					if(pred_count >=2){
						s_prev = addD(BB,global_d,context,sig);
						int d = s_prev ^ sig[&BB];
						Instruction &firstInst = *BB.getFirstNonPHI();
						IRBuilder<> builder(&firstInst);
						Value *currentGValue = builder.CreateLoad(intType,global_g);
						Value *newGValue = builder.CreateXor(currentGValue, ConstantInt::get(intType, d));
						builder.CreateStore(newGValue, global_g);
						Value *currentDValue = builder.CreateLoad(intType,global_d);
						currentGValue = builder.CreateLoad(intType,global_g);
						newGValue = builder.CreateXor(currentGValue, currentDValue);
						builder.CreateStore(newGValue, global_g);
						Value *block_sign = ConstantInt::get(intType, sig[&BB]);
						std::vector<Value*> args = {newGValue, block_sign};
						builder.CreateCall(M.getFunction("error_checker"), args);
					}
					count++;
				}
			}
		}
		void create_checker_function(Module &M, LLVMContext &context)
		{
			FunctionType* funcType = FunctionType::get(Type::getVoidTy(context), {IntegerType::getInt32Ty(context), IntegerType::getInt32Ty(context)}, false);
			Function* errorCheckerFunc = Function::Create(funcType, Function::LinkageTypes::ExternalLinkage, "error_checker", &M);
			BasicBlock* entryBlock = BasicBlock::Create(context, "entry", errorCheckerFunc);

			// Create IR builder
			IRBuilder<> builder(entryBlock);
			Value* a = errorCheckerFunc->arg_begin();
			Value* b = std::next(errorCheckerFunc->arg_begin());
			// Compare a != b
			Value* cmp = builder.CreateICmpNE(a, b, "cmp");
			// Create if-else structure
			BasicBlock* thenBlock = BasicBlock::Create(context, "then", errorCheckerFunc);
			BasicBlock* endBlock = BasicBlock::Create(context, "end", errorCheckerFunc);
			builder.CreateCondBr(cmp, thenBlock, endBlock);
			// 'then' block: printf("illegal branch\n")
			builder.SetInsertPoint(thenBlock);
			Constant* format_str = builder.CreateGlobalStringPtr("illegal branch\n");
			Value *printfArgs[] = {format_str};
			builder.CreateCall(M.getFunction("printf"), printfArgs);
			builder.CreateBr(endBlock);
			// End block
			builder.SetInsertPoint(endBlock);
			builder.CreateRetVoid();

		}
		PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
		{
			LLVMContext &context = M.getContext();
			insert_printf(M,context);
			create_checker_function(M,context);
			map<BasicBlock*,int> sig = calculate_signature(M);
			basic_instruct_cfcss(M);
			fan_instruct_cfcss(M,sig);
			return PreservedAnalyses::none();
		}
	};
 

} 

llvm::PassPluginLibraryInfo CFCSSPluginInfo()
{
  	return {LLVM_PLUGIN_API_VERSION, "CFCSS", LLVM_VERSION_STRING,[](PassBuilder &PB)
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
  return CFCSSPluginInfo();
}