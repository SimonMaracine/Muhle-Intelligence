use std::sync::atomic::AtomicBool;
use std::sync::atomic::Ordering;
use std::sync::Arc;
use std::cmp;
use std::time::Instant;

use crate::evaluation;
use crate::game;
use crate::messages;
use crate::move_generation;

pub struct ThinkContext {
    nodes: i32,
    should_stop: Arc<AtomicBool>,
    can_stop: bool,
    time_begin: Instant,
    max_time: u32,
}

impl ThinkContext {
    pub fn new(should_stop: Arc<AtomicBool>, max_time: u32) -> Self {
        Self {
            nodes: 0,
            should_stop,
            can_stop: false,
            time_begin: Instant::now(),
            max_time,
        }
    }

    fn stop(&self) -> bool {
        self.should_stop.load(Ordering::SeqCst) && self.can_stop
    }

    fn check_time(&self, time_now: Instant) {
        if (time_now - self.time_begin).as_millis() >= self.max_time as u128 {
            self.should_stop.store(true, Ordering::SeqCst);
        }
    }
}

pub struct Think<'a> {
    nodes: Vec<game::SearchNode<'a>>,
}

impl<'a> Think<'a> {
    pub fn new() -> Self {
        Self {
            nodes: Vec::new(),
        }
    }

    pub fn think(mut self, game: game::Game, mut ctx: ThinkContext) -> Option<game::Move> {
        let current_node = self.setup(&game.position, &game.moves);
        let mut last_pv_line = game::PvLine::new();

        let begin = Instant::now();

        for depth in 1..=Self::max_depth(&game) {
            let mut line = game::PvLine::new();

            let eval = Self::alpha_beta(
                &mut ctx,
                depth,
                0,
                evaluation::WINDOW_MIN,
                evaluation::WINDOW_MAX,
                current_node,
                &mut line,
            );

            let now = Instant::now();

            // Check the time here too
            ctx.check_time(now);

            if ctx.stop() {
                // Discard the PV
                break;
            }

            last_pv_line = line.clone();

            let _ = messages::info(
                Some(depth),
                Some((now - begin).as_millis().try_into().expect("Should fit")),
                Some(ctx.nodes),
                Some(Self::score(eval * evaluation::perspective(&current_node.position.position))),
                None,
                None,
                None,
                None,
                Some(&line),
            );

            if line.size == 0 {
                return None;
            }

            // There is a best move available only after the first iteration
            ctx.can_stop = true;
        }

        assert!(last_pv_line.size > 0);

        Some(last_pv_line.moves[0])
    }

    fn alpha_beta(
        ctx: &mut ThinkContext,
        depth: i32,
        depth_root: i32,
        mut alpha: evaluation::Eval,
        beta: evaluation::Eval,
        current_node: &game::SearchNode,
        p_line: &mut game::PvLine,
    ) -> evaluation::Eval {
        if ctx.nodes % 50_000 == 0 {
            ctx.check_time(Instant::now());
        }

        if ctx.stop() {
            return 0;
        }

        if current_node.position.position.is_game_over_material() {  // Game over
            ctx.nodes += 1;
            p_line.size = 0;
            return evaluation::MIN + depth_root;  // Encourage winning earlier
        }

        let moves = move_generation::generate_moves(current_node);

        if moves.is_empty() {  // Game over
            ctx.nodes += 1;
            p_line.size = 0;
            return evaluation::MIN + depth_root;  // Encourage winning earlier
        }

        if current_node.is_threefold_repetition_rule() {  // Game over
            ctx.nodes += 1;
            p_line.size = 0;
            return 0;
        }

        if current_node.position.position.is_fifty_move_rule() {  // Game over
            ctx.nodes += 1;
            p_line.size = 0;
            return 0;
        }

        // The game is not over
        if depth == 0 {
            ctx.nodes += 1;
            p_line.size = 0;
            return evaluation::static_evaluation(current_node) * evaluation::perspective(&current_node.position.position);
        }

        let mut line = game::PvLine::new();

        for move_ in moves {
            let mut new_node = game::SearchNode::from_node(current_node);
            new_node.play_move(&move_);

            let eval = -Self::alpha_beta(ctx, depth - 1, depth_root + 1, -beta, -alpha, &new_node, &mut line);

            if ctx.stop() {
                return 0;
            }

            if eval >= beta {
                return beta;
            }

            if eval > alpha {
                alpha = eval;

                Self::fill_pv(p_line, &line, &move_);
            }
        }

        return alpha;
    }

    fn setup(&mut self, position: &game::Position, moves: &Vec<game::Move>) -> &game::SearchNode<'a> {
        let mut game_position = game::GamePosition {
            position: position.clone(),
            plies_no_advancement: 0,
        };

        self.nodes.push(game::SearchNode::from_position(&game_position));

        for move_ in moves {
            game_position.play_move(move_);
            self.nodes.push(game::SearchNode::from_position(&game_position));  // FIXME
        }

        self.nodes.last().expect("There should be at least one node")
    }

    fn max_depth(game: &game::Game) -> i32 {
        if let Some(max_depth) = game.max_depth {
            return cmp::min(max_depth, game::MAX_DEPTH);
        }

        game::MAX_DEPTH
    }

    fn fill_pv(p_line: &mut game::PvLine, line: &game::PvLine, move_: &game::Move) {
        p_line.moves[0] = *move_;

        for i in 0..line.size {
            p_line.moves[i + 1] = line.moves[i];
        }

        p_line.size = line.size + 1;
    }

    fn score(eval: evaluation::Eval) -> messages::Score {
        assert!(eval <= evaluation::MAX && eval >= evaluation::MIN);

        if eval >= evaluation::MAX - game::MAX_DEPTH {
            let moves = (evaluation::MAX - eval) / 2;
            messages::Score::Win(moves)
        } else if eval <= evaluation::MIN + game::MAX_DEPTH {
            let moves = (evaluation::MIN - eval) / 2;
            messages::Score::Win(moves)
        } else {
            messages::Score::Eval(eval)
        }
    }
}
