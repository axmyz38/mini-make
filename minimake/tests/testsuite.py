import subprocess
import pathlib
import sys
import yaml

# Chemins
ROOT_DIR = pathlib.Path(__file__).resolve().parent.parent
MINIMAKE_BIN = ROOT_DIR / "minimake"
TEST_DIR = ROOT_DIR / "tests"
YAML_FILE = TEST_DIR / "tests.yaml"


def run_test(test):
    args = test.get("args", [])
    expected_exit = test.get("exit_code", 0)
    output_file = test.get("output_file")

    if not output_file:
        print("[ERROR] Missing output_file in test definition")
        return False

    expected_path = ROOT_DIR / output_file

    if not expected_path.exists():
        print(f"[ERROR] Expected file not found: {output_file}")
        return False

    try:
        result = subprocess.run(
            [str(MINIMAKE_BIN)] + args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        output = result.stdout
        expected_output = expected_path.read_text()

        success = True

        if result.returncode != expected_exit:
            print(f"[FAIL] args={args}")
            print(f"Exit code mismatch: expected {expected_exit}, got {result.returncode}")
            success = False

        if output != expected_output:
            print(f"[FAIL] args={args}")
            print("---- Expected ----")
            print(expected_output)
            print("---- Got ----")
            print(output)
            success = False

        if success:
            print(f"[OK] args={args}")

        return success

    except Exception as e:
        print(f"[ERROR] args={args}: {e}")
        return False


def main():
    if not MINIMAKE_BIN.exists():
        print("minimake binary not found")
        sys.exit(1)

    if not YAML_FILE.exists():
        print("tests.yaml not found")
        sys.exit(1)

    with open(YAML_FILE, "r") as f:
        data = yaml.safe_load(f)

    tests = data.get("tests", [])
    if not tests:
        print("No tests found in YAML")
        sys.exit(1)

    success = True

    for test in tests:
        if not run_test(test):
            success = False

    if not success:
        sys.exit(1)


if __name__ == "__main__":
    main()
