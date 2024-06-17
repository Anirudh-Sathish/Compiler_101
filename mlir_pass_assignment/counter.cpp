/*
Counts and prints number of operations, functions and regions
*/
#include "mlir/Dialect/SCF/Transforms/Passes.h"

#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/SCF/Transforms/Transforms.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Support/LLVM.h"
#include "mlir/Pass/PassRegistry.h"
#include "mlir/Pass/PassManager.h" 
#include "mlir/Dialect/Func/IR/FuncOps.h"

namespace mlir {
#define GEN_PASS_DEF_SCFCOUNTER
#include "mlir/Dialect/SCF/Transforms/Passes.h.inc"
} // namespace mlir
using namespace llvm;
using namespace mlir;

namespace
{
  struct OpCounter : public impl::SCFCounterBase<OpCounter>
  {
    unsigned numOperations = 0;
    unsigned numFunctions = 0;
    unsigned numRegions = 0;
    void countRegions(Operation *op)
    {
      // Count the regions directly attached to this operation
      numRegions += op->getNumRegions();

      // Recursively count regions in nested operations
      for (Region &region : op->getRegions()) {
        for (Block &block : region) {
          block.walk([&](Operation *nestedOp) {
            countRegions(nestedOp);
          });
        }
      }
    }

    void runOnOperation() override {
      numOperations = 0;
      numRegions = 0;
      getOperation()->walk([&](Operation *op) {
      if (isa<mlir::func::FuncOp>(op)) {
        numFunctions++;
      }
      numOperations++;
      countRegions(op);
    });

    llvm::errs() << "Number of operations: " << numOperations << "\n";
    llvm::errs() << "Number of functions: " << numFunctions << "\n";
    llvm::errs() << "Number of regions: " << numRegions << "\n";
  }
};
}

std::unique_ptr<Pass> mlir::createCounterPass() {
  return std::make_unique<OpCounter>();
}

