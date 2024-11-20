#ifndef CONSOLE_H
#define CONSOLE_H

#define clear_screen() printf("\033[1;1H\033[2J")
#define move_cursor_to(x,y) printf("\033[%d;%dH", (y), (x))
#define move_cursor_up(x) printf("\033[%dA", (x))
#define move_cursor_down(x) printf("\033[%dB", (x))
#define move_cursor_right(x) printf("\033[%dC", (x))
#define move_cursor_left(x) printf("\033[%dD", (x))

#define color_printf(x, y) printf("%s%s%s", (y), (x), RESET)

#define BLACK    "\033[30m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE     "\033[34m"
#define MAGENTA  "\033[35m"
#define CYAN     "\033[36m"
#define WHITE    "\033[37m"

#define BBLACK   "\033[90m"
#define BRED     "\033[91m"
#define BGREEN   "\033[92m"
#define BYELLOW  "\033[93m"
#define BBLUE    "\033[94m"
#define BMAGENTA "\033[95m"
#define BCYAN    "\033[96m"
#define BWHITE   "\033[97m"

#define RESET    "\033[0m"

#define OWN_COLOR      BBLUE
#define OPPONENT_COLOR MAGENTA

#define ERROR_COLOR       RED
#define INFORMATION_COLOR BYELLOW

#define FRIENDS "\033[36mhttps://www.santemagazine.fr/psycho-sexo/psycho/10-facons-de-se-faire-des-amis-178690\033[0m"

#endif /* guard */