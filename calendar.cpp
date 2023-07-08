#include <string>
#include <stdexcept>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <getopt.h>
#include <ncurses.h>
#include <signal.h>

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

int main(int argc, char **argv)
{
    const int KEY_ESC = 27;
    const int MIN_MONTH = 0;
    const int MAX_MONTH = 11;

    WINDOW *win = NULL;
    t_options options;
    t_date today;

    try
    {        
        if (!get_options(argc, argv, options))
        {
            show_usage(argv[0]);
            return options.error ? 1 : 0;
        }

        signal(SIGINT, finish);

        // Set up ncurses
        initscr();
        cbreak();
        keypad(stdscr, 1);
        curs_set(0);
        noecho();
        start_color();
        refresh();

        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_WHITE, COLOR_RED);

        current_date(today);
        int year = options.year > 0 ? options.year : today.year;
        int month = options.month >= 0 ? options.month : today.month;
        win = show_calendar(year, month);
        
        int ch;
        do
        {
            switch (ch = getch())
            {
            case KEY_RESIZE:
                erase_window(win);
                erase();
                refresh();
                win = show_calendar(year, month);
                break;
            case KEY_HOME:
                win = reshow_calendar(win, today.year, today.month);
                break;
            case KEY_UP:
                if (--month < MIN_MONTH)
                {
                    month = MAX_MONTH;
                    --year;
                }
                win = reshow_calendar(win, year, month);
                break;
            case KEY_DOWN:
                if (++month > MAX_MONTH)
                {
                    month = MIN_MONTH;
                    ++year;
                }
                win = reshow_calendar(win, year, month);
                break;
            case KEY_PPAGE:
                win = reshow_calendar(win, --year, month);
                break;
            case KEY_NPAGE:
                win = reshow_calendar(win, ++year, month);
                break;
            case KEY_ESC:
                break;
            default:
                beep();
                break;
            }
        } while (ch != KEY_ESC);
        delwin(win);
        finish(0);
    }
    catch (const std::exception &e)
    {
        if (win != NULL) delwin(win);
        std::cerr << "Error: " << e.what() << std::endl;
        finish(1);
    }
}

static void finish(int sig)
{
    endwin();
    exit(sig);
}

int day_of_week_number(int day, int month, int year)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year / 4 - year / 100 + year / 400 + t[month] + day) % 7;
}

bool is_leap_year(int year)
{
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int days_in_month_year(int month, int year)
{
    switch (month)
    {
    case 1: // February
        return is_leap_year(year) ? 29 : 28;
    case 3:  // April
    case 5:  // June
    case 8:  // September
    case 10: // November
        return 30;
    default: // All the rest...
        return 31;
    }
}

void mvwputnum(WINDOW *win, int y, int x, int num, int width)
{
    std::ostringstream sstream;
    sstream << std::setw(width) << num << std::ends;
    mvwaddstr(win, y, x, sstream.str().c_str());
}

void erase_window(WINDOW *win)
{
    wbkgd(win, COLOR_PAIR(0));
    werase(win);
    wrefresh(win);
}

WINDOW *create_calendar_window(int height, int width, int y, int x)
{
    WINDOW *win = newwin(height, width, y, x);
    wbkgd(win, COLOR_PAIR(1));
    box(win, 0, 0);
    wrefresh(win);
    return win;
}

std::string format_header(int year, int month)
{
    const std::string months[] = {"January", "February", "March", "April",
                                  "May", "June", "July", "August", "September", "October", "November",
                                  "December"};
    std::ostringstream oss;
    oss << months[month] << " " << year << std::ends;
    return oss.str();
}

inline void crlf(int &row, int &col)
{
    ++row;
    col = 1;
}

int number_of_weeks(int first_dow, int days_in_month)
{
    switch (days_in_month)
    {
    case 28:
        return first_dow == 0 ? 4 : 5;
    case 30:
        return first_dow == 6 ? 6 : 5;
    case 31:
        return (first_dow == 5 || first_dow == 6) ? 6 : 5;
    default:
        return 5;
    }
}

WINDOW *reshow_calendar(WINDOW *win, int year, int month)
{
    erase_window(win);
    delwin(win);
    return show_calendar(year, month);
}

WINDOW *show_calendar(int year, int month)
{
    const int width = 31;
    const int base_height = 5;
    const int day_width = 2;
    const int col_space = 4;

    t_date today;
    current_date(today);
    bool showing_this_month = today.year == year && today.month == month;

    auto first_dow = day_of_week_number(1, month, year);
    int days = days_in_month_year(month, year);
    int height = base_height + number_of_weeks(first_dow, days);

    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    WINDOW *win = create_calendar_window(height, width, starty, startx);

    if (LINES < height || COLS < width)
    {
        erase_window(win);
        attron(COLOR_PAIR(2) | A_BOLD);
        mvaddstr(0, 0, "The screen is too small to show the calendar");
        attroff(COLOR_PAIR(2) | A_BOLD);
        refresh();
        return win;
    }

    int row = 1;
    int col = 1;

    // Show the month and year
    wattron(win, COLOR_PAIR(2) | A_BOLD);
    mvwhline(win, row, col, ' ', width - 2);
    std::string header = format_header(year, month);
    col = (width - header.length()) / 2;
    mvwaddstr(win, row, col, header.c_str());
    wattroff(win, COLOR_PAIR(2) | A_BOLD);

    // Draw a line beneath the header
    wattron(win, COLOR_PAIR(1));
    crlf(row, col);
    mvwaddch(win, row, --col, ACS_LTEE);
    mvwhline(win, row, ++col, ACS_HLINE, width - 2);
    mvwaddch(win, row, width - 1, ACS_RTEE);

    // Show the days of the week
    crlf(row, col);
    std::array<std::string, 7> day_strings {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    wattron(win, COLOR_PAIR(1) | A_UNDERLINE);
    for (size_t i = 0; i < day_strings.size(); ++i)
    {
        mvwaddstr(win, row, col + 1 + i * 4, day_strings[i].c_str());
    }
    wattroff(win, COLOR_PAIR(1) | A_UNDERLINE);

    // Show the days
    auto cell = first_dow;
    crlf(row, col);
    for (int day = 1; day <= days; ++day)
    {
        if (showing_this_month && day == today.day)
        {
            wattron(win, COLOR_PAIR(2) | A_BOLD | A_REVERSE);
            mvwputnum(win, row + (cell / 7), col + day_width + (cell % 7) * col_space, day, day_width);
            wattroff(win, COLOR_PAIR(2) | A_REVERSE);
        }
        else
        {
            wattron(win, COLOR_PAIR(1) | A_BOLD);
            mvwputnum(win, row + (cell / 7), col + day_width + (cell % 7) * col_space, day, day_width);
            wattroff(win, COLOR_PAIR(1) | A_BOLD);
        }
        ++cell;
    }

    attroff(COLOR_PAIR(1));
    wrefresh(win);
    return win;
}

bool get_options(int argc, char **argv, t_options &options)
{
    options.error = false;
    options.help = false;
    options.year = 0;
    options.month = -1;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"year", required_argument, 0, 'y'},
        {"month", required_argument, 0, 'm'},
        {0, 0, 0, 0}};

    int c = 0;
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "hy:m:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'y':
            options.year = std::atoi(optarg);
            break;
        case 'm':
            options.month = (std::atoi(optarg) - 1) % 12;
            break;
        case 'h':
            options.help = true;
            break;
        default:
            options.error = true;
        }
    }
    return !options.error && !options.help;
}

void current_date(t_date &date)
{
    time_t t = time(0);
    tm *timeStruct = localtime(&t);
    date.month = timeStruct->tm_mon;
    date.year = timeStruct->tm_year + 1900;
    date.day = timeStruct->tm_mday;
}

void show_usage(char *program)
{
    std::cout << "Usage: " << program << " [OPTIONS]" << std::endl
              << "Show calendar" << std::endl
              << std::endl
              << "OPTIONS" << std::endl
              << "  -y, --year=NUM   start with year" << std::endl
              << "  -m, --month=NUM  start with month" << std::endl
              << "  -h, --help       display this help text and exit" << std::endl;
}
