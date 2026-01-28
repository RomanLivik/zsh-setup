#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;
using namespace std;

const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string CYAN    = "\033[36m";
const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string INV     = "\033[7m";

struct Strings {
    string lang_name, title_features, title_themes, title_syntax, title_install, hint_nav, backup_msg, omz_check, plugin_download, shell_change, done_msg, restart_msg;
};

map<string, Strings> locales = {
    {"en", {
        "English", " Zsh & Fish-like Features ", " Select Zsh Theme ", " Select Syntax Highlighting Mode ", " Installing Zsh Environment ",
        "Arrows: Navigate | Space: Toggle | Enter: Next",
        "[!] Old config backed up to:", "[*] Installing Oh My Zsh...", "[*] Downloading: ",
        "[*] Changing shell to Zsh...", "─── ZSH CONFIGURED! ───", "Please restart terminal or type 'zsh'"
    }},
    {"ru", {
        "Русский", " Функции Zsh (стиль Fish) ", " Выберите тему Zsh ", " Выберите режим подсветки ", " Установка окружения Zsh ",
        "Стрелки: Навигация | Пробел: Выбор | Enter: Далее",
        "[!] Старый конфиг сохранен по пути:", "[*] Установка Oh My Zsh...", "[*] Загрузка: ",
        "[*] Смена оболочки на Zsh...", "─── ZSH НАСТРОЕН! ───", "Перезапустите терминал или введите 'zsh'"
    }}
};

struct Option { string label_en, label_ru, id; bool selected; };
struct ZshTheme { string name, config_val, repo; };
struct SyntaxMode { string name_en, name_ru, repo, plugin_name; };

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void clear() { cout << "\033[H\033[2J"; }

class ZshConfigurator {
    Strings s;
    string lang = "en";
    vector<Option> options;
    vector<ZshTheme> themes;
    vector<SyntaxMode> syntax_modes;
    int current_theme = 0;
    int current_syntax = 0;
    string home_dir;

public:
    ZshConfigurator() {
        const char* h = getenv("HOME");
        home_dir = h ? string(h) : ".";

        options = {
            {"Fish-like Autosuggestions", "Автодополнение как в Fish", "autosuggest", true},
            {"Tab Completion Menu", "Меню выбора (Tab) со стрелками", "menu", true},
            {"Auto-install completions", "Доп. скрипты автодополнения", "completions", true},
            {"Useful Aliases", "Полезные алиасы (ls, gs, c)", "aliases", true}
        };

        themes = {
            {"robbyrussell (Classic)", "robbyrussell", ""},
            {"Powerlevel10k (Modern)", "powerlevel10k/powerlevel10k", "https://github.com/romkatv/powerlevel10k.git"},
            {"Agnoster", "agnoster", ""},
            {"Pure", "pure", "https://github.com/sindresorhus/pure.git"}
        };

        syntax_modes = {
            {"Standard (zsh-syntax-highlighting)", "Стандартная подсветка синтаксиса", "https://github.com/zsh-users/zsh-syntax-highlighting.git", "zsh-syntax-highlighting"},
            {"Diverse Palette (fast-syntax-highlighting)", "Разнообразная палитра (FSH)", "https://github.com/zdharma-continuum/fast-syntax-highlighting.git", "fast-syntax-highlighting"}
        };
    }

    void draw_header(string title) {
        cout << BLUE << "╭──────────────────────────────────────────────────╮" << endl;
        cout << "│ " << BOLD << CYAN << title << RESET << BLUE << endl;
        cout << "╰──────────────────────────────────────────────────╯" << RESET << endl;
    }

    void run() {
        int cursor = 0;
        vector<string> l_keys = {"en", "ru"};
        while(true) {
            clear();
            cout << BLUE << "╭──────────────────────────╮" << endl;
            cout << "│    Zsh Ultimate Setup    │" << endl;
            cout << "╰──────────────────────────╯" << RESET << endl;
            for(int i=0; i<2; ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << locales[l_keys[i]].lang_name << " " << RESET << endl;
                else cout << "    " << locales[l_keys[i]].lang_name << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': cursor = 0; break; case 'B': cursor = 1; break; } }
            else if(c == 10) { lang = l_keys[cursor]; s = locales[lang]; break; }
        }

        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_features);
            for(int i=0; i<(int)options.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV; else cout << "    ";
                cout << "[" << (options[i].selected ? "X" : " ") << "] " << (lang=="ru"?options[i].label_ru:options[i].label_en) << RESET << endl;
            }
            cout << "\n" << YELLOW << s.hint_nav << RESET;
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<(int)options.size()-1) cursor++; break; } }
            else if(c == ' ') options[cursor].selected = !options[cursor].selected;
            else if(c == 10) break;
        }

        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_themes);
            for(int i=0; i<(int)themes.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << themes[i].name << " " << RESET << endl;
                else cout << "    " << themes[i].name << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<(int)themes.size()-1) cursor++; break; } }
            else if(c == 10) { current_theme = cursor; break; }
        }

        cursor = 0;
        while(true) {
            clear(); draw_header(s.title_syntax);
            for(int i=0; i<(int)syntax_modes.size(); ++i) {
                if(i == cursor) cout << YELLOW << "  ▸ " << INV << " " << (lang=="ru"?syntax_modes[i].name_ru:syntax_modes[i].name_en) << " " << RESET << endl;
                else cout << "    " << (lang=="ru"?syntax_modes[i].name_ru:syntax_modes[i].name_en) << endl;
            }
            int c = getch();
            if(c == 27) { getch(); switch(getch()) { case 'A': if(cursor>0) cursor--; break; case 'B': if(cursor<(int)syntax_modes.size()-1) cursor++; break; } }
            else if(c == 10) { current_syntax = cursor; break; }
        }

        install();
    }

    void install() {
        clear();
        draw_header(s.title_install);
        string zrc = home_dir + "/.zshrc";
        string custom_p = home_dir + "/.oh-my-zsh/custom/plugins";

        if (fs::exists(zrc)) {
            string backup_path = zrc + ".backup";
            fs::copy_file(zrc, backup_path, fs::copy_options::overwrite_existing);
            cout << YELLOW << s.backup_msg << " " << BOLD << fs::absolute(backup_path) << RESET << endl;
        }

        if (!fs::exists(home_dir + "/.oh-my-zsh")) {
            cout << GREEN << s.omz_check << RESET << endl;
            system("sh -c \"$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)\" \"\" --unattended > /dev/null 2>&1");
        }

        cout << CYAN << s.plugin_download << syntax_modes[current_syntax].plugin_name << RESET << endl;
        system(("git clone " + syntax_modes[current_syntax].repo + " " + custom_p + "/" + syntax_modes[current_syntax].plugin_name + " > /dev/null 2>&1").c_str());

        if (options[0].selected) { 
            cout << CYAN << s.plugin_download << "zsh-autosuggestions" << RESET << endl;
            system(("git clone https://github.com/zsh-users/zsh-autosuggestions.git " + custom_p + "/zsh-autosuggestions > /dev/null 2>&1").c_str());
        }
        if (options[2].selected) { 
            cout << CYAN << s.plugin_download << "zsh-completions" << RESET << endl;
            system(("git clone https://github.com/zsh-users/zsh-completions.git " + custom_p + "/zsh-completions > /dev/null 2>&1").c_str());
        }

        if (!themes[current_theme].repo.empty()) {
            cout << CYAN << s.plugin_download << themes[current_theme].name << RESET << endl;
            string t_name = themes[current_theme].config_val;
            if (t_name.find('/') != string::npos) t_name = t_name.substr(t_name.find_last_of('/') + 1);
            system(("git clone --depth=1 " + themes[current_theme].repo + " " + home_dir + "/.oh-my-zsh/custom/themes/" + t_name + " > /dev/null 2>&1").c_str());
        }

        ofstream f(zrc);
        f << "export ZSH=\"$HOME/.oh-my-zsh\"" << endl;
        
        string theme_val = themes[current_theme].config_val;
        if (theme_val.find('/') != string::npos) theme_val = theme_val.substr(theme_val.find_last_of('/') + 1);
        f << "ZSH_THEME=\"" << theme_val << "\"" << endl;

        f << "plugins=(git sudo extract z " << syntax_modes[current_syntax].plugin_name;
        if(options[0].selected) f << " zsh-autosuggestions";
        if(options[2].selected) f << " zsh-completions";
        f << ")" << endl;

        if(options[2].selected)
            f << "fpath+=${ZSH_CUSTOM:-${ZSH:-~/.oh-my-zsh}/custom}/plugins/zsh-completions/src" << endl;

        f << "source $ZSH/oh-my-zsh.sh" << endl;

        if(options[1].selected) {
            f << "\n# Fish-like tab menu" << endl;
            f << "zstyle ':completion:*' menu select" << endl;
            f << "zstyle ':completion:*' matcher-list 'm:{a-zA-Z}={A-Za-z}' 'r:|=*' 'l:|=* r:|=*'" << endl;
            f << "autoload -Uz compinit && compinit" << endl;
        }

        if(options[3].selected) {
            f << "\nalias ll='ls -lah'\nalias gs='git status'\nalias c='clear'" << endl;
        }

        if (themes[current_theme].config_val.find("powerlevel10k") != string::npos)
            f << "[[ ! -f ~/.p10k.zsh ]] || source ~/.p10k.zsh" << endl;

        f.close();

        cout << YELLOW << s.shell_change << RESET << endl;
        system("chsh -s $(which zsh) > /dev/null 2>&1");

        cout << endl << BOLD << GREEN << s.done_msg << RESET << endl;
        cout << s.restart_msg << endl;
    }
};

int main() {
    ZshConfigurator c;
    c.run();
    return 0;
}