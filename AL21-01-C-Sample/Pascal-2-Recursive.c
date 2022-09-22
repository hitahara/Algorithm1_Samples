/// gcc (Ubuntu 9.3.0-17ubuntu~20.04)9 9.3.0
/// 以下のURLでの環境でも実行確認可能．
/// https://paiza.io/ja/languages/online-c-compiler

#include <stdio.h>

int gcd(int m, int n)
{
  if (n == 0)
  {
    return m;
  }
  else
  {
    return gcd(n, m % n);
  }
}

int main()
{
  int m, n, gcd_answer;

  printf("Type in m...\n");
  scanf("%d", &m);
  printf("Type in n...\n");
  scanf("%d", &n);

  gcd_answer = gcd(m, n);

  printf("GCD: %d.\n", gcd_answer);

  return 0;
}
