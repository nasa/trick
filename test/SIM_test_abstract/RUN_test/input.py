
def main():
    ball.ball_ptr = trick.TMM_declare_var_s("Soccerball[1]")
    ball.ball_ptr.thisown = 0
    ball.ball_ptr[0].print_type()
    ball.ball_ptr.print_type()
    trick.add_read(0.5 , 'ball.ball_ptr = trick.TMM_declare_var_s("Baseball[1]")')

if __name__ == "__main__":
    main()


