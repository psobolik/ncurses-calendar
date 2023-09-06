typedef struct
{
    int year;
    int month;
    int day;
} t_date;

typedef struct
{
    int year;
    int month;
    bool error;
    bool help;
} t_options;

bool get_options(int argc, char **argv, t_options &options);
void show_usage(char *program);
void current_date(t_date &date);
void erase_window(WINDOW *win);
WINDOW *create_calendar_window(int height, int width, int y, int x);
WINDOW *reshow_calendar(WINDOW *win, int year, int month);
WINDOW *show_calendar(int year, int month);
int day_of_week_number(int day, int month, int year);
bool is_leap_year(int year);
int days_in_month_year(int month, int year);
void mvwputnum(WINDOW *win, int y, int x, int num, int width);
std::string format_header(int year, int month);
static void finish(int sig);

