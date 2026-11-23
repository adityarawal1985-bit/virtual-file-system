#include "phase3_rounak_cli.hpp"
int main(){
    setupFileSystem();
    journal.load();
    runPhase3Shell();
    return 0;
}
