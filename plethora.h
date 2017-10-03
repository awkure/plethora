#pragma once

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9) || defined(__clang__)
#  warning "Your version of GCC does not support some attributes"
#  pragma GCC optimize push
#  pragma GCC optimize ("-O2")
#endif

#define _VERSION "0.0.1.2"

#define EDITOR_TAB_WIDTH            8
#define EDITOR_STATUS_BAR           1
#define EDITOR_SYNTAX_HIGHLIGHTING  0
#define EDITOR_WARN_UNSAVED         1
#define EDITOR_NOTIFICATION_TIME    3

__BEGIN_DECLS

#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))

#define __st(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __mba(a) BUILD_BUG_ON_ZERO(__st((void *)&(a), &(a)[0]))

#define SIZE(a) (sizeof(a) / sizeof(a[0]) + __mba(a))
#define MAX(a,b) \
  ({ __auto_type _a = (a); \
     __auto_type _b = (b); \
     (_a > _b) ? _a : _b; })

#define DIE(err) (fatal(__func__, err))

#define ABUF_INIT {NULL, 0}
#define CTRL_KEY(k) ((k) & 0x1f)

#define E_DIRT (1<<0)
#define E_LN   (1<<1)
#define E_HL   (1<<2)

#define E_HL_NUMBERS (1<<0)
#define E_HL_STRINGS (1<<1)
#define E_HL_KEYMULT (1<<2)

#define HLDB_TYPES (SIZE(HLDB))

#define APPD(ab, s) (ab_appd(ab, s, SIZE(s)))

enum e_key {
    BACKSPACE = 127
  , ARROW_UP = 1401
  , ARROW_LEFT
  , ARROW_DOWN 
  , ARROW_RIGHT
  , PAGE_UP
  , PAGE_DOWN
  , DEL_KEY
  , HOME_KEY
  , END_KEY
};

enum e_mode {
      E_NORMAL_MODE
    , E_INSERT_MODE
    , E_GUERILLA_MODE
};

enum e_hl {
      HL_NORMAL
    , HL_COMMENT
    , HL_MLCOMMENT
    , HL_NUMBER
    , HL_MATCH
    , HL_KEYWORD1
    , HL_KEYWORD2
    , HL_KEYWORD3
};

enum e_clr {
      C_BLACK  = 30
    , C_RED 
    , C_GREEN
    , C_YELLOW 
    , C_BLUE
    , C_MAGENTA
    , C_CYAN
    , C_GREY
    , C_NORMAL = 0
};


typedef struct {
    size_t sz;
    size_t rsz; 
    ifast32 clns;
    char cln[sizeof(int)];
    char * ch;
    char * rch; 
    unsigned char * hl;
} e_row;


struct e_syn {
    char *  restrict ft;
    char ** restrict fm;
    char ** restrict kw;
    char *  restrict sl_com_st;
    char *  restrict ml_com_st;
    char *  restrict ml_com_end;
    uleast8 flags;
}; 

static struct {
    ifast32 cx, cy;
    ifast32 rx;
    ifast32 rscr; 
    ifast32 cscr; 
    i32 term_rows;
    i32 term_cols;
    intptr file_nrows;
    e_row * row;
    uleast8 mode;
    uleast8 flags;
    char * restrict fn; 
    char * restrict ext; 
    char sb_msg[100]; 
    time_t sb_time;
    struct e_syn * restrict syn;
    struct termios ot; 
} EC = {0}; 

struct abuf {
    char * restrict b;
    ifast16 l;
};

extern void e_init(void);

extern int get_window_size(int *, int *); 
extern int get_cursor_pos(int *, int *);

extern void enable_raw_mode(void);
extern void disable_raw_mode(void);

extern void e_help(void);
extern void e_open(char *);
extern void e_write(void);
extern void e_find_cb(char *, int);
extern void e_find(void);
extern char * e_prompt(char const *, void (*)(char *, int));

extern void e_toggle_ln(void);
extern void e_upd_screen(void);
extern void e_clear_screen(void);
extern void e_draw_rows(struct abuf *);
extern void e_draw_sb(struct abuf *);
extern void e_draw_mb(struct abuf *);
extern void e_welcome(struct abuf *);
extern void e_set_sb_msg(const char *, ...); 
extern void e_appd_row(int, char *, size_t);
extern void e_appd_new_line(void);
extern void e_upd_row(e_row *);
extern void e_free_row(e_row *);
extern void e_del_row(int);
extern void e_row_appd_str(e_row *, char *, size_t);
extern int  e_row_cx_rx(e_row *, int);
extern int  e_row_rx_cx(e_row *, int);

extern void e_row_in_char(e_row *, i16, int);
extern void e_row_del_char(e_row *, i16);
extern void e_in_char(int);
extern void e_del_char(void);

extern int  e_read_key(void);
extern void e_move_cursor(int);
extern void e_on_keypress(void);

extern void ab_appd(struct abuf *, const char *, int);
//extern inline void ab_free(struct abuf *);
extern void ab_free(struct abuf *);


extern void e_toggle_hl(void);
extern void e_upd_hl(e_row *);
extern void e_find_syn_hl(void);
extern int  e_syn_to_clr(int);
extern char e_is_sep(int);

extern void fatal(const char *, char *);

__END_DECLS
