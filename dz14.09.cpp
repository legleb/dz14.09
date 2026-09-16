#include <iostream>
#include <vector>
#include <cstddef>
#include <exception>
#include <chrono>
#include <future>
#include <string>
#include <functional>

class Clicker {
  public:
    Clicker():
      start_(std::chrono::high_resolution_clock::now())
    {}
    double millisec() const {
      using std::chrono::high_resolution_clock;
      using std::chrono::duration_cast;
      using std::chrono::milliseconds;
      auto t = high_resolution_clock::now();
      return duration_cast< milliseconds >(t - start_).count();
    }
  private:
    std::chrono::time_point< std::chrono::steady_clock > start_;
};


using data_t = std::vector< unsigned long long >;
using value_t = data_t::value_type;

value_t sum(const data_t & vals, size_t st, size_t lng) {
  value_t sm = 0;
  for (size_t i = st; i < st + lng; ++i) {
    sm += vals[i];
  }
  return sm;
}

int main(int argc, char * argv[]) {
  if (argc < 2) {
    std::cerr << "Ошибка! Не введено кол-во потоков\n";
    return 1;
  } else if (argc > 2) {
    std::cerr << "Ошибка! Введено много аргументов\n";
    return 1;
  }
  size_t countPotoc = 0;
  try {
    countPotoc = static_cast< size_t >(std::stoull(argv[1]));
    if (!countPotoc) {
      throw std::invalid_argument("Ошибка! Кол-во потоков - это положительное число");
    }
  }
  catch (const std::exception & e) {
    std::cerr << "Ошибка! " << e.what() << "\n";
    return 2;
  }
  constexpr size_t size{100000000};
  data_t values(size, 1);
  std::vector< std::future< value_t > > ftrs;
  ftrs.reserve(countPotoc);
  size_t baseSizePart = size / countPotoc;
  size_t dopSizePart = size % countPotoc;
  size_t idx = 0;
  double total{0};
  value_t result = 0;
  {
    Clicker cl;
    for (size_t i = 0; i < countPotoc; ++i) {
      size_t sizePart = baseSizePart + (i < dopSizePart ? 1 : 0);
      ftrs.emplace_back(std::async(std::launch::async, sum, std::cref(values), idx, sizePart));
      idx += sizePart;
    }
    for (size_t i = 0; i < countPotoc; ++i) {
      result += ftrs[i].get();
    }
    total = cl.millisec();
  }
  std::cout << "Кол-во потоков: " << countPotoc << "\nСумма: " << result << "\nВремя: " << total << " мс\n";
  return 0;
}
