#ifndef GAME_H
#define GAME_H

#define GRID_SIZE 10
#define PADDLE_SIZE 3

typedef struct {
    int ball_x, ball_y;
    int ball_dx, ball_dy;
    int paddle_left, paddle_right;
    int score_left, score_right;
} GameState;

void update_ball(GameState *state);
void reset_ball(GameState *state);
void initialize_game(GameState *state);
void print_grid(GameState* state);
#endif
