import subprocess

replacement_policies = ["LRU", "RANDOM", "TREELRU"]
prefetcher_settings = [True, False]

# Program executable (update this path if necessary)
program_path = "./cache"

def run_simulation(policy, prefetcher_enabled):
    # Set environment variables or command-line arguments as needed
    # (assuming the program accepts them as command-line arguments)
    cmd = [
        program_path,
        f"--policy={policy}",
        f"--prefetcher={'enabled' if prefetcher_enabled else 'disabled'}"
    ]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        output = result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running simulation with policy={policy} prefetcher_enabled={prefetcher_enabled}")
        print(e)
        return
    
    filename = f"Results{policy}{'Enabled' if prefetcher_enabled else 'Disabled'}.txt"
    
    with open(filename, "w") as file:
        file.write(output)
    print(f"Saved results to {filename}")

for policy in replacement_policies:
    for prefetcher_enabled in prefetcher_settings:
        run_simulation(policy, prefetcher_enabled)
