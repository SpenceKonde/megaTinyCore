import sys
import time


class ProgressBar:
    def __init__(self, n_steps, width=50, hide=False):
        self.width = width
        self.n_steps = n_steps
        self.count_step = 0
        self.count_char = 0
        self.hide = hide
        self.print_start()

    def print_start(self):
        if not self.hide:
            sys.stdout.write("[%s]" % (" " * self.width))
            sys.stdout.flush()

    def print_end(self):
        if not self.hide:
            sys.stdout.write("\n")
            sys.stdout.flush()

    def update(self):
        n1 = self.count_char
        n2 = self.width - self.count_char
        if not self.hide:
            sys.stdout.write("\r[" + "=" * n1 + " " * n2 + "] {}/{}".format(self.count_step, self.n_steps))
            sys.stdout.flush()

    def step(self):
        self.count_step += 1
        count_char_new = self.width * self.count_step // self.n_steps

        if count_char_new > self.count_char:
            self.count_char = count_char_new
            self.update()

        if self.count_step == self.n_steps:
            self.print_end()


def test():
    n_steps = 100

    print("Starting...")

    b = ProgressBar(n_steps)

    for i in range(n_steps):
        time.sleep(0.01)
        b.step()

    print("done.")


if __name__ == "__main__":
    test()
