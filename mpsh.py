import numpy as np
import random

# -------------------------
# Game Setup and Ship Placement Rules
# -------------------------

ships = [4, 3, 3, 2, 2, 2, 1, 1, 1, 1]
# ships = [5,4,3,3,2]

def draw_grid(height, width):
    return np.zeros((height, width), dtype=int)

def is_valid_placement(grid, x, y, ship, direction):
    height, width = grid.shape
    if direction == 0:  # Horizontal
        if x + ship > width:
            return False  # Out of bounds
        if np.any(grid[y, max(0, x - 1):min(width, x + ship + 1)]):
            return False
        if y > 0 and np.any(grid[y - 1, max(0, x - 1):min(width, x + ship + 1)]):
            return False
        if y < height - 1 and np.any(grid[y + 1, max(0, x - 1):min(width, x + ship + 1)]):
            return False
    else:  # Vertical
        if y + ship > height:
            return False
        if np.any(grid[max(0, y - 1):min(height, y + ship + 1), x]):
            return False
        if x > 0 and np.any(grid[max(0, y - 1):min(height, y + ship + 1), x - 1]):
            return False
        if x < width - 1 and np.any(grid[max(0, y - 1):min(height, y + ship + 1), x + 1]):
            return False
    return True

def place_ships(grid, ships):
    for ship in ships:
        while True:
            x = np.random.randint(0, grid.shape[1])
            y = np.random.randint(0, grid.shape[0])
            direction = np.random.randint(0, 2)
            if is_valid_placement(grid, x, y, ship, direction):
                if direction == 0:
                    grid[y, x:x + ship] = 1
                else:
                    grid[y:y + ship, x] = 1
                break
    return grid

# -------------------------
# Display and Ship Sinking Detection
# -------------------------

def print_player_grid(grid, hit_coords, miss_coords, sunk_coords):
    height, width = grid.shape
    for y in range(height):
        for x in range(width):
            if (x, y) in sunk_coords:
                print('S', end=' ')
            elif (x, y) in hit_coords:
                print('X', end=' ')
            elif (x, y) in miss_coords:
                print('O', end=' ')
            else:
                print('#', end=' ')
        print()

def is_sunk(grid, hit_coords, x, y):
    ship_coords = [(x, y)]
    height, width = grid.shape
    for i in range(x - 1, -1, -1):
        if grid[y, i] == 1:
            ship_coords.append((i, y))
        else:
            break
    for i in range(x + 1, width):
        if grid[y, i] == 1:
            ship_coords.append((i, y))
        else:
            break
    for j in range(y - 1, -1, -1):
        if grid[j, x] == 1:
            ship_coords.append((x, j))
        else:
            break
    for j in range(y + 1, height):
        if grid[j, x] == 1:
            ship_coords.append((x, j))
        else:
            break
    return all(coord in hit_coords for coord in ship_coords), ship_coords

# -------------------------
# Marking Adjacent Cells as Misses
# -------------------------

def mark_adjacent_as_miss(ship_coords, hit_coords, sunk_coords, miss_coords, height, width):
    for (x, y) in ship_coords:
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                nx = x + dx
                ny = y + dy
                if 0 <= nx < width and 0 <= ny < height:
                    if (nx, ny) not in hit_coords and (nx, ny) not in sunk_coords and (nx, ny) not in miss_coords:
                        miss_coords.append((nx, ny))

# -------------------------
# Enhanced Monte Carlo Simulation and Heuristics
# -------------------------

def create_bot_game_state(hit_coords, miss_coords, sunk_coords, height, width):
    state = np.zeros((height, width), dtype=int)
    for (x, y) in hit_coords:
        state[y, x] = 1
    for (x, y) in sunk_coords:
        state[y, x] = 1
    for (x, y) in miss_coords:
        state[y, x] = -1
    return state

def generate_random_board_consistent_with(game_state, ships):
    height, width = game_state.shape
    board = np.zeros((height, width), dtype=int)
    for ship in ships:
        placed = False
        attempts = 0
        while not placed and attempts < 100:
            attempts += 1
            direction = random.choice([0, 1])
            if direction == 0:
                x = random.randint(0, width - ship)
                y = random.randint(0, height - 1)
            else:
                x = random.randint(0, width - 1)
                y = random.randint(0, height - ship)
            if not is_valid_placement(board, x, y, ship, direction):
                continue
            candidate = [(x + i, y) for i in range(ship)] if direction == 0 else [(x, y + i) for i in range(ship)]
            if any(game_state[cy, cx] == -1 for (cx, cy) in candidate):
                continue
            for (cx, cy) in candidate:
                board[cy, cx] = 1
            placed = True
        if not placed:
            return None
    for y in range(height):
        for x in range(width):
            if game_state[y, x] == 1 and board[y, x] != 1:
                return None
    return board

def monte_carlo_heatmap(game_state, ships, num_simulations=1000):
    height, width = game_state.shape
    heatmap = np.zeros((height, width), dtype=float)
    valid_simulations = 0
    for _ in range(num_simulations):
        sim_board = generate_random_board_consistent_with(game_state, ships)
        if sim_board is None:
            continue
        valid_simulations += 1
        heatmap += sim_board
    if valid_simulations > 0:
        heatmap = heatmap / valid_simulations
    return heatmap

# --- Calculate Placement Density ---

def calculate_placement_density(game_state, ship_length):
    """Count every candidate placement for a ship of length ship_length that does not conflict with known misses."""
    height, width = game_state.shape
    density = np.zeros((height, width))
    # Horizontal placements
    for y in range(height):
        for x in range(width - ship_length + 1):
            candidate = [(x + i, y) for i in range(ship_length)]
            if all(game_state[cy, cx] != -1 for (cx, cy) in candidate):
                for (cx, cy) in candidate:
                    density[cy, cx] += 1
    # Vertical placements
    for x in range(width):
        for y in range(height - ship_length + 1):
            candidate = [(x, y + i) for i in range(ship_length)]
            if all(game_state[cy, cx] != -1 for (cx, cy) in candidate):
                for (cx, cy) in candidate:
                    density[cy, cx] += 1
    return density

def calculate_total_placement_density(game_state, ships):
    """Sum up placement densities for each ship."""
    total_density = np.zeros(game_state.shape)
    for ship in ships:
        total_density += calculate_placement_density(game_state, ship)
    return total_density

# --- Dynamically Adjust Simulations and Combine Heuristics ---

def monte_carlo_heatmap_weighted_dynamic(game_state, ships, base_simulations=5000, multiplier=50, alpha=0.7):
    """
    Increase the number of simulations based on the number of unknown cells,
    then combine the MC heatmap with the placement density map.
    A higher base_simulations and multiplier are used to optimize for fewer guesses.
    """
    unknown_cells = np.sum(game_state == 0)
    num_simulations = base_simulations + multiplier * unknown_cells
    heatmap_mc = monte_carlo_heatmap(game_state, ships, num_simulations)
    
    placement_density = calculate_total_placement_density(game_state, ships)
    if np.max(placement_density) > 0:
        placement_density = placement_density / np.max(placement_density)
    
    combined = alpha * heatmap_mc + (1 - alpha) * placement_density
    return combined

def choose_next_move(game_state, heatmap):
    height, width = game_state.shape
    best_score = -1
    best_move = None
    for y in range(height):
        for x in range(width):
            if game_state[y, x] != 0:
                continue
            if heatmap[y, x] > best_score:
                best_score = heatmap[y, x]
                best_move = (x, y)
    return best_move

# --- Target Mode: When a hit exists, preferentially target its neighbors ---

def target_mode_move(game_state, hit_coords, height, width):
    """Return the unknown neighbor (up/down/left/right) of any hit (not yet sunk) with the highest placement density."""
    candidate_moves = []
    for (x, y) in hit_coords:
        for dx, dy in [(0,1), (1,0), (0,-1), (-1,0)]:
            nx = x + dx
            ny = y + dy
            if 0 <= nx < width and 0 <= ny < height:
                if game_state[ny, nx] == 0:  # unknown cell
                    candidate_moves.append((nx, ny))
    if candidate_moves:
        density = calculate_total_placement_density(game_state, ships)
        best = None
        best_val = -1
        for move in candidate_moves:
            val = density[move[1], move[0]]
            if val > best_val:
                best_val = val
                best = move
        return best
    else:
        return None

# --- Monte Carlo Bot (with Target Mode) ---

def monte_carlo_bot(game_state, ships, base_simulations=5000, multiplier=50, alpha=0.7, height=10, width=10, hit_coords_global=None, sunk_coords_global=None):
    """
    First, if there are any hits not yet sunk, use target mode.
    Otherwise, use the heavy MC + density combined approach.
    """
    # If any hit remains (that is not part of a sunk ship), try to target its neighbors.
    if hit_coords_global is not None and sunk_coords_global is not None:
        valid_hits = [h for h in hit_coords_global if h not in sunk_coords_global]
        if valid_hits:
            target_move = target_mode_move(game_state, valid_hits, height, width)
            if target_move is not None:
                return target_move, None

    combined_heatmap = monte_carlo_heatmap_weighted_dynamic(game_state, ships, base_simulations, multiplier, alpha)
    return choose_next_move(game_state, combined_heatmap), combined_heatmap

# -------------------------
# Main Game Loops (Auto and Feedback Modes)
# -------------------------

def play_battleship_auto(grid):
    hit_coords = []
    miss_coords = []
    sunk_coords = []
    height, width = grid.shape
    num_guesses = 0
    while len(sunk_coords) < sum(ships):
        print_player_grid(grid, hit_coords, miss_coords, sunk_coords)
        bot_state = create_bot_game_state(hit_coords, miss_coords, sunk_coords, height, width)
        # Pass hit_coords and sunk_coords for target mode consideration.
        (x, y), heatmap = monte_carlo_bot(bot_state, ships, height=height, width=width, hit_coords_global=hit_coords, sunk_coords_global=sunk_coords)
        print("Bot guesses:", (x, y))
        if (x, y) in hit_coords or (x, y) in miss_coords:
            print("Already guessed that spot!")
            continue
        if grid[y, x] == 1:
            print("Hit!")
            hit_coords.append((x, y))
            sunk, ship_coords = is_sunk(grid, hit_coords, x, y)
            if sunk:
                print("Sunk a ship!")
                sunk_coords.extend(ship_coords)
                mark_adjacent_as_miss(ship_coords, hit_coords, sunk_coords, miss_coords, height, width)
        else:
            print("Miss!")
            miss_coords.append((x, y))
        num_guesses += 1
        print("-" * 30)
    print("All ships sunk. You won! Total guesses:", num_guesses)
    print_player_grid(grid, hit_coords, miss_coords, sunk_coords)

def play_battleship_feedback():
    hit_coords = []
    miss_coords = []
    sunk_coords = []
    height, width = 10, 10
    num_guesses = 0
    print("Feedback mode: after each guess, type HIT, MISS, SINK or DONE (to end).")
    while True:
        bot_state = create_bot_game_state(hit_coords, miss_coords, sunk_coords, height, width)
        (x, y), heatmap = monte_carlo_bot(bot_state, ships, height=height, width=width, hit_coords_global=hit_coords, sunk_coords_global=sunk_coords)
        print("Bot guesses:", (x+1, y+1))
        feedback = input("Enter feedback (HIT, MISS, SINK, or DONE): ").strip().upper()
        if feedback == "DONE":
            break
        elif feedback == "HIT":
            hit_coords.append((x, y))
        elif feedback == "MISS":
            miss_coords.append((x, y))
        elif feedback == "SINK":
            sunk_coords.append((x, y))
            mark_adjacent_as_miss([(x, y)], hit_coords, sunk_coords, miss_coords, height, width)
        else:
            print("Invalid feedback. Please enter HIT, MISS, SINK, or DONE.")
            continue
        num_guesses += 1
        print("Guesses so far:", num_guesses)
        print("-" * 30)
    print("Feedback mode ended. Total guesses:", num_guesses)
    print("Hit coordinates:", hit_coords)
    print("Sunk coordinates:", sunk_coords)
    print("Miss coordinates:", miss_coords)

# -------------------------
# Main Program
# -------------------------

if __name__ == '__main__':
    mode = input("Enter mode (auto/feedback): ").strip().lower()
    if mode == "auto":
        grid = draw_grid(10, 10)
        grid_with_ships = place_ships(grid, ships)
        play_battleship_auto(grid_with_ships)
    elif mode == "feedback":
        play_battleship_feedback()
    else:
        print("Invalid mode.")
