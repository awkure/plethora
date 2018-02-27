#pragma GCC push_options

//#define _DEFAULT_SOURCE
//#define _POSIX_C_SOURCE 200809L //
#define _BSD_SOURCE
#define _GNU_SOURCE
//#define _ATFILE_SOURCE //
//#define _XOPEN_SOURCE //

char * C_HL_EXTS[]  = { ".c", ".h", NULL };
char * J_HL_EXTS[]  = { ".j", ".ijs", NULL };
char * CC_HL_EXTS[] = { ".cc", ".cpp", ".c++", ".cp", ".cxx", ".h", ".hh", NULL};
char * RS_HL_EXTS[] = { ".rs", NULL };
char * JS_HL_EXTS[] = { ".js", ".es6", ".es", NULL };
char * HS_HL_EXTS[] = { ".hs", ".lhs", NULL };
char * ID_HL_EXTS[] = { ".idr", NULL };
char * PY_HL_EXTS[] = { ".py", ".py3", ".ipynb", NULL };
char * LI_HL_EXTS[] = { ".cl", ".lisp", NULL };
char * EL_HL_EXTS[] = { ".el", ".elisp", NULL };
char * PS_HL_EXTS[] = { ".pp", ".pas", ".pascal", NULL };

struct e_syn HLDB[] = {
    {
         "C"
        , C_HL_EXTS
        , NULL
        , "//"
        , "/*", "*/" 
        , E_HL_NUMBERS | E_HL_STRINGS | E_HL_KEYMULT
    },
    {
         "C++"
        , CC_HL_EXTS
        , NULL
        , "//"
        , "/*", "*/"
        , E_HL_NUMBERS | E_HL_STRINGS | E_HL_KEYMULT
    },
    {
         "Rust"
        , RS_HL_EXTS
        , NULL
        , "//"
        , "/*", "*/"
        , E_HL_STRINGS | E_HL_KEYMULT
    },
    {
         "J"
        , J_HL_EXTS
        , NULL
        , "NB."
        , NULL, NULL
        , E_HL_NUMBERS | E_HL_STRINGS 
    },
    {
         "JavaScript"
        , JS_HL_EXTS
        , NULL
        , "//"
        , "/*", "*/"
        , E_HL_NUMBERS 
    },
    {
         "Haskell"
        , HS_HL_EXTS
        , NULL
        , "-- "
        , "{-", "-}"
        , 0 
    },
    {
         "Idris"
        , ID_HL_EXTS
        , NULL
        , "-- "
        , "{-", "-}"
        , 0
    },
    {
         "Python"
        , PY_HL_EXTS
        , NULL
        , "#"
        , NULL, NULL
        , E_HL_NUMBERS | E_HL_STRINGS | E_HL_KEYMULT
    },
    {
         "Lisp"
        , LI_HL_EXTS
        , NULL
        , ";"
        , NULL, NULL
        , 0 
    },
    {
         "Elisp"
        , EL_HL_EXTS
        , NULL
        , ";"
        , NULL, NULL
        , 0 
    },
    {
         "Pascal"
        , PS_HL_EXTS
        , NULL
        , "//"
        , "(*", "*)"
        , E_HL_NUMBERS | E_HL_STRINGS
            
    }
};



int
main(int argc, char **argv)
{
    enable_raw_mode();
    e_init();
    e_help();

    if (argc >= 2)
        e_open(argv[1]);

    for (;;) {
        e_upd_screen();
        e_on_keypress();
    }

    return 0;
}



void 
e_init(void)
{
    EC.fn  = NULL;
    EC.row = NULL;
    EC.syn = NULL;
    EC.mode = E_NORMAL_MODE;
    
    if (get_window_size(&EC.term_rows, &EC.term_cols) == -1) 
        DIE("get_window_size");

    EC.term_rows -= 2;
}



void 
enable_raw_mode(void)
{

    if(tcgetattr(STDIN_FILENO, &EC.ot) == -1) 
        DIE("tcgetattr");

    atexit(disable_raw_mode);

    struct termios raw = EC.ot;
    raw.c_iflag &= ~( ICRNL | IXON | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~( OPOST );
    raw.c_lflag &= ~( ICANON | IEXTEN | ISIG | ECHO );
    raw.c_cflag |=  ( CS8 ); 

    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
        DIE("tcsetattr");
}



void __attribute__((noinline)) 
disable_raw_mode(void)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &EC.ot) == -1)
        DIE("tcsetattr");
}



int
e_read_key(void)
{
    int c;
    i16 n;

    while ((n = read(STDIN_FILENO, &c, 1)) != 1) 
        if (n == -1/* && errno != EAGAIN*/) DIE("read");

    if (c == '\x1b') {
        char s[3];

        // TODO
        if (read(STDIN_FILENO, &s[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &s[1], 1) != 1) return '\x1b';

        if (*s == '[') { 
            if (s[1] >= '0' && s[1] <= '9') {
                if (read(STDIN_FILENO, &s[2], 1) != 1) return '\x1b';
                if (s[2] == '~') 
                    switch (s[1]) {
                        case '1' : return HOME_KEY;
                        case '3' : return DEL_KEY;
                        case '4' : return END_KEY;
                        case '5' : return PAGE_UP;
                        case '6' : return PAGE_DOWN;
                        case '7' : return HOME_KEY;
                        case '8' : return END_KEY;
                    }
            } else 
                switch (s[1]) {
                    case 'A' : return ARROW_UP;
                    case 'B' : return ARROW_DOWN;
                    case 'C' : return ARROW_RIGHT;
                    case 'D' : return ARROW_LEFT;
                    case 'H' : return HOME_KEY;
                    case 'F' : return END_KEY;
                }
        } else if (*s == 'O')
            switch (s[1]) {
                case 'H' : return HOME_KEY;
                case 'F' : return END_KEY;
            }

        return '\x1b';
    }
    return c;
}



int
get_window_size(int * rows, int * cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) 
            return -1;
        return get_cursor_pos(rows, cols);
    }

    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
}



int 
get_cursor_pos(int * rows, int * cols)
{
    char buf[32];
    u32 i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) 
        return -1;

    for (; i < sizeof(buf) - 1; ++i)
        if (read(STDIN_FILENO, &buf[i], 1) != 1 || buf[i] == 'R') 
            break;

    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[' || sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}



inline void
e_help(void)
{
    e_set_sb_msg("Ctrl-? = help │ Ctrl-S = save │ Ctrl-Q = quit │ Ctrl-F = find │ Ctrl-I = Insert mode");
}



void
e_toggle_ln(void)
{
    EC.flags ^= E_LN;
    /* should've saved original cx coordinate or implement an additional buffer */
    EC.cx = (EC.flags & E_LN) ? EC.cx+EC.row[EC.cy].clns+1 : EC.cx - EC.row[EC.cy].clns-1;
    e_set_sb_msg("Line enumeration is ", (EC.flags & E_LN) ? "on" : "off");
}



void
e_on_keypress(void)
{
    int c = e_read_key();
    static i8 qatt = 1;

    switch (c) {
        case '\r' :
            e_appd_new_line();
            break;

        case CTRL_KEY('q') :
            if ((EC.flags & E_DIRT) && qatt > 0 && EDITOR_WARN_UNSAVED) {
                e_set_sb_msg("[!!] File has unsaved changes. Press Ctrl-Q again to quit.");
                qatt--;
                return;
            }
            e_clear_screen();
            exit(0);
            break;

        case CTRL_KEY('s') :
            e_write();
            break;

        case CTRL_KEY('f') :
            e_find();
            break;

        case CTRL_KEY('g') :
            e_toggle_hl();
            break;

        case CTRL_KEY('n') :
            e_toggle_ln();
            break;

        case CTRL_KEY('i') :
            EC.mode = E_INSERT_MODE;
            break;

        case CTRL_KEY('?') :
            e_help();
            break;

        case PAGE_UP   :
        case PAGE_DOWN :
            {
                if (c == PAGE_UP)
                    EC.cy = EC.rscr;
                else if (c == PAGE_DOWN) {
                    EC.cy = EC.rscr + EC.term_rows - 1;
                    if (EC.cy > EC.file_nrows) 
                        EC.cy = EC.file_nrows;
                }

                int n = EC.term_rows;
                while (n--)
                    e_move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
            break;

        case HOME_KEY : EC.cx = 0; break;
        case END_KEY  : 
            if (EC.cy < EC.file_nrows)
                EC.cx = EC.row[EC.cy].sz; 
            break; 

        case BACKSPACE     :
        case CTRL_KEY('h') :
        case DEL_KEY       :
            if (EC.mode == E_INSERT_MODE) {
                if (c == DEL_KEY) 
                    e_move_cursor(ARROW_RIGHT);
                e_del_char();
            }
            break;

        case CTRL_KEY('l') :
        case '\x1b'        :
            EC.mode = E_NORMAL_MODE;
            break;

        case ARROW_UP    :
        case ARROW_LEFT  :
        case ARROW_DOWN  :
        case ARROW_RIGHT :
            e_move_cursor(c);
            break;

        case CTRL_KEY(ARROW_UP)    :
        case CTRL_KEY(ARROW_LEFT)  :
        /* case CTRL_KEY(ARROW_DOWN)  : */
        case CTRL_KEY(ARROW_RIGHT) :
            break;
        
        default :
            if (EC.mode == E_INSERT_MODE)
                e_in_char(c);
            break;
    }

    qatt = 1;
}



void
e_move_cursor(int k)
{
    e_row * r = (EC.cy >= EC.file_nrows) ? NULL : &EC.row[EC.cy];
        
    switch(k) {
        case ARROW_UP    : if (EC.cy != 0)      
                               EC.cy--; 
                           break;
        case ARROW_LEFT  : if (EC.cx != ((EC.flags & E_LN) ? r->clns+1 : 0))
                               EC.cx--; 
                           else if (EC.cy > 0) {
                               EC.cy--;
                               EC.cx = EC.row[EC.cy].sz;
                           }
                           break;
        case ARROW_DOWN  : if (EC.cy < EC.file_nrows) 
                               EC.cy++; 
                           break;
        case ARROW_RIGHT : if (r && (size_t)EC.cx < r->sz)     
                               EC.cx++; 
                           else if (r && (size_t)EC.cx == r->sz) {
                               EC.cy++;
                               EC.cx = (EC.flags & E_LN) ? r->clns+1 : 0;
                           }
                           break;
    }
    r = (EC.cy <= EC.file_nrows) ? &EC.row[EC.cy] : NULL;
    int rl = r ? r->sz : 0;
    EC.cx = (EC.cx > rl) ? rl : EC.cx;
}



char * __attribute__((__format_arg__ (1)))
e_prompt(char const * p, void (*cb)(char *, int))
{
    size_t bsz = 128, bl = 0;
    char * b = calloc(1, bsz);
    b[0] = '\0';

    for (;;) {
        e_set_sb_msg(p, b);
        e_upd_screen();

        int c = e_read_key();
        if (c == '\x1b') {
            e_set_sb_msg("");
            if (cb)
                cb(b, c);
            free(b);
            return NULL;
        } else if (c == DEL_KEY 
                || c == CTRL_KEY('h') 
                || c == BACKSPACE) {
            if (bl != 0)
                b[--bl] = '\0';
        } else if (c == '\r') {
            if (bl != 0) {
                e_set_sb_msg("");
                if (cb)
                    cb(b, c);
                return b;
            }
        } else if (!iscntrl(c) && c < 128) {
            if (bl == bsz - 1) {
                bsz *= 2;
                b = realloc(b, bsz);
            }
            b[bl++] = c;
            b[bl] = '\0';
        }
        if (cb)
            cb(b, c);
    }
}



void __attribute__((hot))
e_draw_rows(struct abuf * ab) 
{
    for (i32 r = 0; r < EC.term_rows; ++r) {
        i32 sr = r + EC.rscr;
        
        if ((EC.flags&E_LN) && sr < EC.file_nrows) {
            int w = 0; /* using global EC method causes segfaults inside sptrintf, lol */
            for (int rw = EC.file_nrows; (rw /= 10) != 0; ++w);
            EC.row[r].clns = strlen(EC.row[r].cln);
            sprintf(EC.row[r].cln, "%*li", w+1, r+EC.rscr+1);
            EC.row[r].ch = realloc(EC.row[r].ch, EC.row[r].sz + EC.row[r].clns + 2);
            ab_appd(ab, EC.row[r].cln, strlen(EC.row[r].cln));
            ab_appd(ab, " ", 1);
        }

        if (sr >= EC.file_nrows) {
            if (EC.file_nrows == 0 && r == EC.term_rows / 3) {
                e_welcome(ab);
            } else 
                ab_appd(ab, "~", 1);
        } else {
            size_t ln = EC.row[sr].rsz - EC.cscr > (size_t)EC.term_cols ? 
                (size_t)EC.term_cols : EC.row[sr].rsz - EC.cscr;

            char * c  = &EC.row[sr].rch[EC.cscr];
            u8   * hl = &EC.row[r].hl[EC.cscr];
            for (u32 j = 0, cur_clr = 0; j < ln; ++j) {
                if (hl[j] == HL_NORMAL) {
                    if (cur_clr != 0) {
                        ab_appd(ab, "\x1b[39m", 5);
                        cur_clr = 0;
                    }
                    ab_appd(ab, &c[j], 1);
                } else {
                    int clr = e_syn_to_clr(hl[j]);
                    if ((u32)clr != cur_clr && (EC.flags&E_HL)) {
                        cur_clr = clr;
                        char b[16];
                        //int cln = snprintf(b, sizeof(b), "\x1b[%dm", clr);
                        int cln = sprintf(b, "\x1b[%dm", clr);
                        ab_appd(ab, b, cln);
                    }
                    ab_appd(ab, &c[j], 1);
                }
            }
            ab_appd(ab, "\x1b[39m", 5);
        }
        ab_appd(ab, "\x1b[K", 3);
        ab_appd(ab, "\r\n", 2);
    }
}



void
e_draw_sb(struct abuf * ab)
{
    ab_appd(ab, "\x1b[7m", 4);
    char st[80], rst[80];

    int l = sprintf(st, "[%s] %.20s%1s ", 
            EC.mode ? "INSERT" : "NORMAL", EC.fn ? EC.fn : "[Unnamed]", (EC.flags & E_DIRT) ? "+" : "");

    int rl = sprintf(rst, "%s | c:%02li r:%02li", 
            EC.syn ? EC.syn->ft : "plain text", EC.cx + 1, EC.cy + 1);

    if (l > EC.term_cols) 
        l = EC.term_cols;
    
    ab_appd(ab, st, l);
    
    for (; l < EC.term_cols; ++l) 
        if (EC.term_cols - l - 1 == rl) {
            ab_appd(ab, rst, rl);
            ab_appd(ab, " ", 1);
            break;
        } else 
            ab_appd(ab, " ", 1);
    
    ab_appd(ab, "\x1b[m", 3);
    ab_appd(ab, "\r\n", 2); 
}



void
e_draw_mb(struct abuf * ab)
{
    ab_appd(ab, "\x1b[K", 3);
    int ml = strlen(EC.sb_msg);
    if (ml > EC.term_cols) 
        ml = EC.term_cols;
    if (ml && time(NULL) - EC.sb_time < EDITOR_NOTIFICATION_TIME)
        ab_appd(ab, EC.sb_msg, ml);
}



void __attribute__((hot))
e_scroll(void)
{
    EC.rx = 0;

    if (EC.cy < EC.file_nrows) EC.rx = e_row_cx_rx(&EC.row[EC.cy], EC.cx);

    if (EC.cy < EC.rscr) EC.rscr = EC.cy;
    if (EC.rx < EC.cscr) EC.cscr = EC.rx;
    if (EC.cy >= EC.rscr + EC.term_rows) EC.rscr = EC.cy - EC.term_rows + 1;
    if (EC.rx >= EC.cscr + EC.term_cols) EC.cscr = EC.rx - EC.term_cols + 1;
}



void
e_clear_screen(void)
{
    write(STDOUT_FILENO, "\x1b[2J", 4); 
    write(STDOUT_FILENO, "\x1b[H",  3); 
}



void __attribute__((hot))
e_upd_screen(void)
{
    /*if (get_window_size(&EC.term_rows, &EC.term_cols) == -1) 
        DIE("get_window_size");*/

    e_scroll();

    struct abuf ab = ABUF_INIT;

    ab_appd(&ab, "\x1b[?25l", 6);
    ab_appd(&ab, "\x1b[H"   , 3);

    e_draw_rows(&ab);
    e_draw_sb(&ab);
    e_draw_mb(&ab);

    char b[32];
    //snprintf(b, sizeof(b), "\x1b[%li;%liH", (EC.cy - EC.rscr) + 1, (EC.rx - EC.cscr) + 1);
    sprintf(b, "\x1b[%li;%liH", (EC.cy - EC.rscr) + 1, (EC.rx - EC.cscr) + 1);
    ab_appd(&ab, b, strlen(b));

    ab_appd(&ab, "\x1b[?25h" , 6);

    write(STDOUT_FILENO, ab.b, ab.l);
    ab_free(&ab);
}



void 
e_welcome(struct abuf * ab)
{
    char m[80];
    int ml = 
        sprintf(m, "Plethora -- version %s\n"
                /* "adam <awkure[at]protonmail.ch>\n" 
                "Ctrl-? = help    Ctrl-S = save    Ctrl-Q = quit" */, _VERSION);
    ml = ml > EC.term_cols ? EC.term_cols : ml;

    int pd = (EC.term_cols - ml) / 2; 
    if (pd) {
        ab_appd(ab, "~", 1);
        pd--;
    }
    while (pd--) 
        ab_appd(ab, " ", 1);

    ab_appd(ab, m, ml);
}



void 
e_set_sb_msg(char const * restrict fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    //vsnprintf(EC.sb_msg, sizeof(EC.sb_msg), fmt, ap);
    vsprintf(EC.sb_msg, fmt, ap);
    va_end(ap);
    EC.sb_time = time(NULL);
}



void 
e_upd_row(e_row * r)
{
    int tabs = 0;
    size_t j;
    for (j = 0; j < r->sz; ++j)
        if (r->ch[j] == '\t') 
            tabs++;
    
    free(r->rch);
    r->rch = calloc(r->sz, r->sz + tabs * (EDITOR_TAB_WIDTH - 1) + 1);
 
    int i = 0;
    for (j = 0; j < r->sz; ++j)
        if (r->ch[j] == '\t') {
            r->rch[i++] = ' ';
            while (i % EDITOR_TAB_WIDTH != 0)
                r->rch[i++] = ' ';
        } else {
            r->rch[i++] = r->ch[j];
        }
    r->rch[i] = '\0';
    r->rsz = i;

    //if (EC.flags.hl) 
    e_upd_hl(r); 
}



void 
e_appd_row(int i, char * s, size_t l)
{
    if (i < 0 || i > EC.file_nrows)
        return;
    
    EC.row = realloc(EC.row, sizeof(e_row) * (EC.file_nrows + 1));
    memmove(&EC.row[i+1], &EC.row[i], sizeof(e_row) * (EC.file_nrows-i));

    EC.row[i].sz = l;
    EC.row[i].ch = calloc(1, l+1);
    memcpy(EC.row[i].ch, s, l);
    EC.row[i].ch[l] = '\0';

    EC.row[i].rsz = 0;
    EC.row[i].rch = NULL;
    EC.row[i].hl  = NULL;
    e_upd_row(&EC.row[i]);

    EC.file_nrows++;
    EC.flags |= E_DIRT;
}



inline void
e_free_row(e_row * row) 
{
    free(row->rch);
    free(row->ch);
    free(row->hl);
}



void 
e_del_row(int i)
{
    if (i < 0 || i >= EC.file_nrows)
        return;

    e_free_row(&EC.row[i]);
    memmove(&EC.row[i], &EC.row[i+1], sizeof(e_row) * (EC.file_nrows - i - 1));
    EC.file_nrows--; EC.flags |= E_DIRT;

}



int
e_row_cx_rx(e_row * r, int cx)
{
    int rx = 0;
    for (int j = 0; j < cx; ++j) {
        if (r->ch[j] == '\t')
            rx += (EDITOR_TAB_WIDTH - 1) - (rx % EDITOR_TAB_WIDTH);
        rx++;
    }
    return rx;
}



int 
e_row_rx_cx(e_row * r, int rx) 
{
    size_t cx, crx = 0;
    for (cx = 0; cx < r->sz; ++cx) {
        if (r->ch[cx] == '\t')
            crx += (EDITOR_TAB_WIDTH - 1) - (crx % EDITOR_TAB_WIDTH);
        crx++;
        if (crx > (size_t)rx) 
            return cx;
    }
    return cx;
}



void
e_row_appd_str(e_row * row, char * s, size_t l)
{
    row->ch = realloc(row->ch, row->sz + l + 1);
    memcpy(&row->ch[row->sz], s, l);
    row->sz += l;
    row->ch[row->sz] = '\0';
    e_upd_row(row);
    EC.flags |= E_DIRT;
}



void
e_row_in_char(e_row * row, i16 i, int c)
{
    if (i < 0 || (size_t)i > row->sz) 
        i = row->sz;
    row->ch = realloc(row->ch, row->sz + 2);
    memmove(&row->ch[i+1], &row->ch[i], row->sz - i + 1);
    row->sz++;
    row->ch[i] = c;
    e_upd_row(row);
    EC.flags |= E_DIRT;
}



void
e_in_char(int c)
{
    if (EC.cy == EC.file_nrows) 
        e_appd_row(EC.file_nrows, "", 0);
    e_row_in_char(&EC.row[EC.cy], (EC.flags & E_LN) ? EC.cx-EC.row[EC.cy].clns-1 : EC.cx, c);
    EC.cx++;
}



void
e_row_del_char(e_row * row, i16 i)
{
    if (i < 0 || (size_t)i >= row->sz)
        return;
    
    memmove(&row->ch[i], &row->ch[i+1], row->sz - i);
    row->sz--;
    e_upd_row(row);
    
    EC.flags |= E_DIRT;
}



void
e_del_char(void)
{
    if ((EC.cx == 0 && EC.cy == 0) || EC.cy == EC.file_nrows)
        return;

    e_row * row = &EC.row[EC.cy];
    if (EC.cx > 0) {
        e_row_del_char(row, (EC.flags & E_LN) ? EC.cx-EC.row[EC.cy].clns-2 : EC.cx - 1);
        EC.cx--;
    } else {
        EC.cx = EC.row[EC.cy-1].sz;
        e_row_appd_str(&EC.row[EC.cy-1], row->ch, row->sz);
        e_del_row(EC.cy);
        EC.cy--;
    }
} 



void
e_appd_new_line(void)
{
    if (EC.cx == 0)
        e_appd_row(EC.cy, "", 0);
    else {
        e_row * row = &EC.row[EC.cy];
        e_appd_row(EC.cy+1, &row->ch[EC.cx], row->sz - EC.cx);
        
        row = &EC.row[EC.cy];
        row->sz = EC.cx;
        row->ch[row->sz] = '\0';
        
        e_upd_row(row);
    }
    EC.cy++;
    EC.cx = 0;
}



void __attribute__((nonnull (1)))
e_open(char * fn)
{
    free(EC.fn);
    EC.fn = strdup(fn);  

    if (EC.fn == NULL)
        fatal(__FUNCTION__, EC.fn);


    fd_t * fp = fopen(fn, "r");
    if (!fp) 
        DIE("fopen");

    if (EDITOR_SYNTAX_HIGHLIGHTING)
        e_find_syn_hl();
    
    char *  li = NULL; 
    size_t  lc = 0;
    ssize_t ln;

    while ((ln = getline(&li, &lc, fp)) != -1) {
        while (ln > 0 && (li[ln-1] == '\n' || li[ln-1] == '\r'))
            ln--;
        e_appd_row(EC.file_nrows, li, ln);
    }
    EC.flags &= ~E_DIRT;
    
    free(li);
    fclose(fp);
}


char * 
e_rows_to_str(int * bl)
{
    int tl = 0;
    int j;
    
    for (j = 0; j < EC.file_nrows; ++j)
        tl += EC.row[j].sz + 1;
    *bl = tl;

    char * buf = calloc(EC.file_nrows, tl);
    char * p = buf;

    for (j = 0; j < EC.file_nrows; ++j) {
        memcpy(p, EC.row[j].ch, EC.row[j].sz);
        p += EC.row[j].sz;
        *p = '\n';
        p++;
    }

    return buf;
}



void 
e_write(void)
{
    if (EC.fn == NULL) {
        EC.fn = e_prompt("Filename: %s", NULL);
        if (EC.fn == NULL) {
            return;
        }
        e_find_syn_hl();
    }

    int l;
    char * buf = e_rows_to_str(&l);
    int fd = open(EC.fn, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, l) != -1 && write(fd, buf, l)) {
            close(fd);
            free(buf);
            EC.flags &= ~E_DIRT;
            e_set_sb_msg("\"%s\" %dL, %dC has been written", EC.fn, EC.file_nrows, l);
            return;
        }
        close(fd);
    }
    free(buf);
    e_set_sb_msg("I/O error: %s", /* strerror(errno) */ "shit happens");
}



void
e_find_cb(char * q, int k)
{
    static int l_m = -1;
    static int dir =  1;

    if (k == '\r' || k == '\x1b') {
        l_m = -1;
        dir =  1;
        return;
    } 
    else if (k == ARROW_RIGHT || k == ARROW_DOWN) dir =  1;
    else if (k == ARROW_LEFT  || k == ARROW_UP)   dir = -1;
    else {
        l_m = -1;
        dir =  1;
    }

    dir = (l_m == -1) ? 1 : -1;
    int cur = l_m;
    for (int i = 0; i < EC.file_nrows; ++i) {
        cur += dir;
        cur = (cur == -1) ? EC.file_nrows - 1 : (cur == EC.file_nrows) ? 0 : cur;

        e_row * row = &EC.row[cur];
        char * m = strstr(row->rch, q);
        if (m) {
            l_m     = cur;
            EC.cy   = cur;
            EC.cx   = e_row_rx_cx(row, m - row->rch);
            EC.rscr = EC.file_nrows;

            memset(&row->hl[m-row->rch], HL_MATCH, strlen(q));
            break;
        }
    }
}



void 
e_find(void)
{
    int ocx   = EC.cx;
    int ocy   = EC.cy;
    int orscr = EC.rscr;
    int ocscr = EC.cscr;

    char * q = e_prompt("Search: %s", e_find_cb);
    
    if (!q) { 
        EC.cx   = ocx;
        EC.cy   = ocy;
        EC.rscr = orscr;
        EC.cscr = ocscr;
    } else 
        free(q);
}



void
ab_appd(struct abuf * ab, char const * s, int l)
{
    char * r = realloc(ab->b, ab->l + l);

    if (r == NULL) 
        return;

    memcpy(&r[ab->l], s, l);
    ab->b  = r;
    ab->l += l;
}



inline void __attribute__((__gnu_inline__)) 
ab_free(struct abuf * ab)
{
    free(ab->b);
}



void 
fatal(const char * f, char * err)
{
    e_clear_screen();

    struct abuf eb = ABUF_INIT;

    ab_appd(&eb, "error: ", 7);
    ab_appd(&eb, f, SIZE(f));
    ab_appd(&eb, ": ", 2);
    ab_appd(&eb, err, SIZE(err));
    ab_appd(&eb, "\r\b", 2);

    write(2, &eb, SIZE(eb.b));

    ab_free(&eb);
    exit(1);
}



void
e_toggle_hl(void)
{
    EC.flags ^= E_HL;
    e_set_sb_msg("Guerilla mode is %s", (EC.flags & E_HL) ? "on" : "off");

}



inline char 
e_is_sep(int c)
{
    return strchr(" \t\n\v\r\f,.+-/*=~%()<>[];", c) != NULL || c == '\0'; 
}



void
e_upd_hl(e_row * r)
{
    r->hl = realloc(r->hl, r->rsz);
    memset(r->hl, HL_NORMAL, r->rsz);

    if (EC.syn == NULL)
        return;

    int pr_sp = 1;
    size_t i = 0;
    while (i < r->sz) {
        char c = r->rch[i];
        u8 pr_hl = (i > 0) ? r->hl[i-1] : HL_NORMAL;

        if (EC.syn->flags & E_HL_NUMBERS)
            if ((isdigit(c) && (pr_sp || pr_hl == HL_NUMBER)) 
                    || (c == '.' && pr_hl == HL_NUMBER)) {
                r->hl[i] = HL_NUMBER;
                i++;
                pr_sp = 0;
                continue;
            }
        pr_sp = e_is_sep(c);
        ++ i;
    }
}



int 
e_syn_to_clr(int hl)
{
    switch (hl) {
        case HL_NUMBER : return C_BLUE;
        case HL_MATCH  : return C_YELLOW; 
        default        : return C_NORMAL;
    }
}



void 
e_find_syn_hl(void)
{
    EC.syn = NULL;
    if (EC.fn == NULL)
        return;

    char * ext = strrchr(EC.fn, '.');

    for (u32 j = 0; j < HLDB_TYPES; ++j) {
        struct e_syn * s = &HLDB[j];
        u32 i = 0;
        
        while (s->fm[i]) {
            int is_ext = (s->fm[i][0] == '.');
            if ((is_ext && ext && !strcmp(ext, s->fm[i])) 
            || (!is_ext && strstr(EC.fn, s->fm[i]))) {
                EC.syn = s;
                for (int fr = 0; fr < EC.file_nrows; ++fr)
                    e_upd_hl(&EC.row[fr]);
                return;
            }
            i++;
        }
    }
}

#pragma GCC pop_options
