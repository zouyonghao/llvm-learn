#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/Instructions.h>
#include <string>
#include <vector>
#include <llvm/Support/FileSystem.h>

using namespace llvm;
using namespace std;

static cl::opt<std::string>
        InputFileName(cl::Positional,
                      cl::desc("<input IR file>"),
                      cl::init("-"));

int main(int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv);

    LLVMContext Ctx;
    SMDiagnostic SMDiag;
    auto Mptr = parseIRFile(InputFileName, SMDiag, Ctx);
    if (!Mptr) {
        errs() << SMDiag.getMessage() << "\n";
        return 1;
    }
    vector<Instruction *> instruction_replace;
    vector<Instruction *> instruction_new;
    for (auto &func_i : *Mptr) {
        outs() << func_i.getFunction().getName() << "\n";
        for (Function::iterator bb_i = func_i.begin(); bb_i != func_i.end(); bb_i++) {
            auto *bb = dyn_cast<BasicBlock>(bb_i);
            for (auto &i_i : bb->getInstList()) {
                if (!i_i.isBinaryOp()) {
                    continue;
                }
                auto *i = dyn_cast<Instruction>(&i_i);
                if (i != nullptr && i->getOpcode() == Instruction::Add) {
                    Instruction *newi = BinaryOperator::CreateSub(i->getOperand(0),
                                                                  i->getOperand(1));
                    // ReplaceInstWithInst(i, newi);
                    instruction_replace.push_back(i);
                    instruction_new.push_back(newi);
                }
            }
        }
    }
    for (int i = 0; i < instruction_replace.size(); i++) {
        ReplaceInstWithInst(instruction_replace[i], instruction_new[i]);
    }
    Mptr->print(outs(), nullptr);
    std::error_code ec;
    raw_ostream *out = new raw_fd_ostream(StringRef("test_result.ll"), ec, sys::fs::F_None);
    Mptr->print(*out, nullptr);
    return 0;
}
