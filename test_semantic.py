import subprocess
import os

def run_command(command):
    """
    Run a shell command and print the output.
    """
    try:
        result = subprocess.run(command, shell=True, text=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {command}")
        print(e.stderr)
        raise

def main():
    # Step 1: Run make command to compile cminus_semantic
    print("Running make to compile cminus_semantic...")
    run_command("make cminus_semantic MAIN=test")
    
    # Step 2: Run semantic analysis
    print("--- SEMANTIC ANALYSIS ---")
    
    test_cases = []
    for i in range(1, 9):
      test_cases.append((f"cases/test/semantic/test.{i}.txt", f"cases/result/semantic/result.{i}.txt"))
    
    for idx, (input_file, output_file) in enumerate(test_cases, start=1):
        print(f"Processing COMPILE {idx}...", end="")
        run_command(f"./cminus_semantic {input_file} {output_file}")
        print(f"COMPILE {idx} COMPLETE\n")
    run_command(f"make clean")
    
if __name__ == "__main__":
    main()