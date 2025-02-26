#include <iostream>
#include <vector>
#include <cstring>
// #include <arm_neon.h>
#include <sstream>
#include <algorithm>
#include <chrono>

void batchOutput(const std::vector<int> &data)
{
  std::ostringstream out;
  for (size_t i = 0; i < data.size(); ++i)
  {
    out << data[i] << '\n';
  }
  std::cout << out.str();
  std::cout.flush();
}

void vectorizedRadixSort32_serial(int *data, size_t n)
{
  int *in = data;
  int *out = new int[n];

  constexpr int n_passes = 4;
  constexpr int n_buckets = 256;

  constexpr int mask = 0xFF;

  for (int pass = 0; pass < n_passes; pass++)
  {
    const int shift = 8 * pass;
    int hist[n_buckets] = {0};
    for (size_t i = 0; i < n; i++)
    {
      hist[(in[i] >> shift) & mask] += 1;
    }

    int idx[n_buckets] = {0};
    for (int j = 1; j < n_buckets; j++)
    {
      idx[j] = idx[j - 1] + hist[j - 1];
    }

    for (size_t i = 0; i < n; i++)
    {
      int d = (in[i] >> shift) & mask;
      out[idx[d]++] = in[i];
    }

    std::swap(in, out);
  }

  for (size_t i = 0; i < n; i++)
  {
    in[i] ^= 0x80000000;
  }
  delete[] out;
}

int main()
{
  std::vector<int> data;
  data.reserve(1000000);
  int x;

  constexpr u_int32_t sign_bit = 0x80000000;
  while (std::cin >> x)
  {
    data.push_back(x ^ sign_bit);
  }

  auto start_time = std::chrono::high_resolution_clock::now();
  vectorizedRadixSort32_serial(data.data(), data.size());
  auto end_time = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> elapsed = end_time - start_time;
  std::cerr << "Sorting completed in " << elapsed.count() << " seconds.\n";

  batchOutput(data);

  return 0;
}