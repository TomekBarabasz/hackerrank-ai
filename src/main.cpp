#include <iostream>
#include <cstring>

int cleaning_bot_permute_main(int argc, char**argv);
int cleaning_bot_search_main(int argc, char**argv);
int pacman_main(int argc, char**argv);

using namespace std;
int main(int argc, char**argv)
{
    if (argc > 1) {
        if (0 == strcmp(argv[1], "cbp")) return cleaning_bot_permute_main(argc-2, argv+2);
        if (0 == strcmp(argv[1], "cbs")) return cleaning_bot_search_main (argc-2, argv+2);
        if (0 == strcmp(argv[1], "pac")) return pacman_main (argc-2, argv+2);
    }
    else{
        cout << "avaliable options: " << endl;
        cout << "cbp : cleaning_bot_permute" << endl;
        cout << "cbs : cleaning_bot_search" << endl;
        cout << "pac : pacman" << endl;
    }
    return 1;
}
