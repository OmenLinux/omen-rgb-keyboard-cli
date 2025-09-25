#include "commands.hpp"

int main(int argc, char *argv[]) {
  try {
    omen::rgb::commands::execute(argc, argv);
    return 0;
  } catch (const std::exception &ex) {
    std::cout << "Error:\n  " << ex.what() << std::endl;
  }
}
