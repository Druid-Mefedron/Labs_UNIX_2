import time


def calculate(x):
    return x ** 2 - x ** 2 + x * 4 - x * 5 + x + x


def main():
    while True:
        user_input = input("Enter number of iterations: ").strip()
        try:
            n = int(user_input)
        except ValueError:
            print("Input is not a number, exiting.")
            break

        start = time.perf_counter()
        total = 0
        for i in range(n):
            x = i + 1
            total += calculate(x)
        end = time.perf_counter()
        elapsed_ms = (end - start) * 1000

        print(f"Sum: {total}, iterations: {n}, time: {elapsed_ms:.5f} ms")

        again = input("Repeat? (y/n): ").strip()
        if again.lower() != "y":
            break


if __name__ == "__main__":
    main()
