#include "frontend.h"
#include "backend.h"

int main(void)
{   
    int firstRun = 1;

    while (1)
    {
        set_up_console(firstRun);
        run_game();
        firstRun = 0;
    }
    
    return 0;
}