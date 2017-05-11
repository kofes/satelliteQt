#include "inc/Satellite.hpp"

std::queue< std::pair<short, short> > satellite::getPixelsInLine ( short x0, short y0, short x1, short y1 ) {
  unsigned short dx = std::abs(x1 - x0);
  unsigned short dy = std::abs(y1 - y0);
  char signX = x0 < x1 ? 1 : -1;
  char signY = y0 < y1 ? 1 : -1;
  short err = dx - dy;
  std::queue< std::pair<short, short> > result;

  while ( x0 != x1 || y0 != y1 ) {
    result.push(std::make_pair(x0, y0));

    short err2 = err*2;
    if (err2 > -dy) {
      err -= dy;
      x0 += signX;
    }
    if (err2 < dx) {
      err += dx;
      y0 += signY;
    }
  }
  result.push(std::pair<short, short>(x1, y1));

  return result;
};

std::queue< std::pair<short, short> > satellite::getPixelsInCircle ( short x0, short y0, short r ) {
  int x = 0;
  int y = r;
  int delta = 1 - 2*r;
  int err = 0;
  std::queue< std::pair<short, short> > result;
  while (y >= 0) {
    result.push(std::make_pair(x0 + x, y0 + y));
    result.push(std::make_pair(x0 + x, y0 - y));
    result.push(std::make_pair(x0 - x, y0 + y));
    result.push(std::make_pair(x0 - x, y0 - y));
      err = 2 * (delta + y) - 1;
      if ((delta < 0) && (err <= 0)) {
          delta += 2 * ++x + 1;
          continue;
      }
      err = 2 * (delta - x) - 1;
      if ((delta > 0) && (err > 0)) {

          delta += 1 - 2 * --y;
          continue;
      }
      x++;
      delta += 2 * (x - y);
      y--;
  }
  return result;
};

satellite::math::Pack satellite::math::Pack::calc (  short x0, short y0, short x1, short y1, double h, satellite::Image& picture  ) {
  double m_h, m_0, s_0, s_h;
  m_h = m_0 = s_0 = s_h = 0;
  unsigned int count = 0;
  _err = false;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0) {
    _err = true;
    return *this;
  }
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return *this;

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_0 = (picture[j][i] < 0) ? 0 : picture[j][i];
          short point_h = (picture[j+y][i+x] < 0) ? 0 : picture[j+y][i+x];
          _drift += point_0 - point_h;
          _covariance += point_0 * point_h;
          _semivariance += (point_0 - point_h)*(point_0 - point_h);
          m_0 += point_0;
          m_h += point_h;
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) {
    m_0 /= count;
    m_h /= count;
    _semivariance /= 2 * count;
    _drift /= count;
    _covariance /= count;
    _covariance -= m_0 * m_h;
  }

  return *this;
};

double satellite::math::m ( short x0, short y0, short x1, short y1, satellite::Image& picture) {
  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;

  if (!(x1 - x0) || !(y1 - y0))
    return 0;

  double result = 0;
  std::queue< std::pair<short, short> > que = getPixelsInLine(x0, y0, x1, y1);

  while (!que.empty()) {
    auto pr = que.front();
    que.pop();
    short point = (picture[pr.first][pr.second] < 0) ? 0 : picture[pr.first][pr.second];
    result += point;
  }
  result /= std::sqrt((y1 - y0)*(y1 - y0) + (x1 - x0)*(x1 - x0));

  return result;
};

double satellite::math::m0 ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_0 = (picture[j][i] < 0) ? 0 : picture[j][i];
          result += point_0;
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) result /= count;

  return result;
};

double satellite::math::mh ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

    for (int j = y0; j < y1; ++j)
      for (int i = x0; i < x1; ++i) {
        int x = 0;
        int y = h;
        int delta = 1 - 2*h;
        int err = 0;
        while (y >= 0) {
          if (j + y < y1 && i + x < x1) {
            ++count;
            short point_h = (picture[j+y][i+x] < 0) ? 0 : picture[j+y][i+x];
            result += point_h;
          }
          err = 2 * (delta + y) - 1;
          if ((delta < 0) && (err <= 0)) {
            delta += 2* ++x + 1;
            continue;
          }
          err = 2 * (delta - x) - 1;
          if ((delta > 0) && (err > 0)) {
            delta += 1 - 2* --y;
            continue;
          }
          x++;
          delta +=2*(x-y);
          y--;
        }
      }

  if (count) result /= count;

  return result;
};

double satellite::math::s0 ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

  double dm = satellite::math::m0(x0, y0, x1, y1, h, picture);

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_0 = (picture[j][i] < 0) ? 0 : picture[j][i];
          result += (point_0 - dm)*(point_0 - dm);
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) result /= count;

  return result;
};

double satellite::math::sh ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

  double dm = satellite::math::mh(x0, y0, x1, y1, h, picture);

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_h = (picture[j+y][i+x] < 0) ? 0 : picture[j+y][i+x];
          result += (point_h - dm)*(point_h - dm);
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) result /= count;

  return result;
};

double satellite::math::d ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_0 = (picture[j][i] < 0) ? 0 : picture[j][i];
          short point_h = (picture[j+y][i+x] < 0) ? 0 : picture[j+y][i+x];
          result += point_h - point_0;
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) result /= count;

  return result;
};

double satellite::math::cov ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_0 = (picture[j][i] < 0) ? 0 : picture[j][i];
          short point_h = (picture[j+y][i+x] < 0) ? 0 : picture[j+y][i+x];
          result += point_0 * point_h;
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) {
    result /= count;
    result -= satellite::math::m0(x0, y0, x1, y1, h, picture) * satellite::math::mh(x0, y0, x1, y1, h, picture);
  }

  return result;
};

double satellite::math::r ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double dv = std::sqrt(satellite::math::s0(x0, y0, x1, y1, h, picture) * satellite::math::sh(x0, y0, x1, y1, h, picture));
  if (std::abs(dv) < 1e-5)
    return 0;
  return satellite::math::cov(x0, y0, x1, y1, h, picture) / dv;
};

double satellite::math::g ( short x0, short y0, short x1, short y1, double h, satellite::Image& picture ) {
  double result = 0;
  unsigned int count = 0;

  if (x0 < 0 || x1 > picture.width() || y0 < 0 || y1 > picture.height() || x1 < 0 || y1 < 0)
    return -1;
  if (h <= 1e-5 || !(x1 - x0) || !(y1 - y0))
    return 0;

  for (int j = y0; j < y1; ++j)
    for (int i = x0; i < x1; ++i) {
      int x = 0;
      int y = h;
      int delta = 1 - 2*h;
      int err = 0;
      while (y >= 0) {
        if (j + y < y1 && i + x < x1) {
          ++count;
          short point_0 = (picture[j][i] < 0) ? 0 : picture[j][i];
          short point_h = (picture[j+y][i+x] < 0) ? 0 : picture[j+y][i+x];
          result += (point_0 - point_h)*(point_0 - point_h);
        }
        err = 2 * (delta + y) - 1;
        if ((delta < 0) && (err <= 0)) {
          delta += 2* ++x + 1;
          continue;
        }
        err = 2 * (delta - x) - 1;
        if ((delta > 0) && (err > 0)) {
          delta += 1 - 2* --y;
          continue;
        }
        x++;
        delta +=2*(x-y);
        y--;
      }
    }

  if (count) result /= 2 * count;

  return result;
};

std::vector<double> satellite::math::leastSquares ( unsigned long int degree, std::list<double> x, std::list<double> y, double maxDiff ) {
  if (!x.size() || !y.size() || x.size() != y.size() || maxDiff <= 0)
    return std::vector<double>();

  //mat*ans = res

  std::vector<double> ans, res, tmp;
  std::vector< std::vector<double> > mat;
  ans.resize(degree + 1);
  res.resize(degree + 1);
  tmp.resize(degree + 1);
  mat.resize(degree + 1);
  for (unsigned int i = 0; i <= degree; ++i)
    mat[i].resize(degree + 1);

  //fill left and bottom of matrics 'mat' and fill vector 'res'
  for (unsigned int i = 0; i <= degree; ++i) {
    std::list<double>::iterator it_x, it_y;
    for (it_x = x.begin(), it_y = y.begin(); it_x != x.end() && it_y != y.end(); ++it_x, ++it_y)
      if (std::abs(*it_x) > 1e-5 || !(degree-i)) {
        double tmp_x = std::pow(*it_x, degree-i) / x.size();
        //set res
        res[i] += tmp_x * (*it_y);
        //set left and bottom
        mat[degree][i] += tmp_x;
        mat[i][0] += std::pow(*it_x, 2*degree - i) / x.size();
      }
  }
  mat[degree][0] /= 2;
  mat[degree][degree] = 1;

  //fill all
  for (unsigned int j = 1; j <= degree; ++j)
    for (unsigned int i = 0; i < degree; ++i)
      mat[i][j] = mat[i+1][j-1];

  //find 'ans'
  double diff = 1;
  while (diff > maxDiff) {
    tmp = ans;
    diff = 0;
    for (unsigned int i = 0; i <= degree; ++i) {
      double a1, a2, err;
      a1 = a2 = err = 0;
      for (unsigned int j = 0; j < i; ++j) {
        double buff, dt;
        dt = mat[i][j]*ans[j] - err;
        buff = a1 + dt;
        err = (buff - a1) - dt;
        a1 = buff;
      }
      err = 0;
      for (unsigned int j = i+1; j <= degree; ++j) {
        double buff, dt;
        dt = mat[i][j]*tmp[j] - err;
        buff = a2 + dt;
        err = (buff - a2) - dt;
        a2 = buff;
      }
      ans[i] = (res[i] - a1 - a2) / mat[i][i];
      if (std::abs(ans[i] - tmp[i]) > diff)
        diff = std::abs(ans[i] - tmp[i]);
    }
  }

  return ans;
};

double satellite::math::moment ( const std::vector< std::pair<double, unsigned long> >& x, double center, unsigned short degree, double min, double max ) {
  if (!x.size()) return 0;
  if (!degree) return 1;

  double res = 0, acc = 0, err = 0;
  size_t i = 1;

  for (auto it_x : x) {
    if (it_x.first > max || it_x.first < min)
      continue;

    double buff, dx;

    dx = it_x.second - err;
    buff = acc + dx;
    err = (buff - acc) - dx;
    acc = buff;
  }

  err = 0;
  for (auto it_x : x) {
    if (it_x.first > max || it_x.first < min)
      continue;

    double buff, dx;

    dx = std::pow(it_x.first - center, degree) * (it_x.second/acc) - err;
    buff = res + dx;
    err = (buff - res) - dx;
    res = buff;
    i++;
  }

  return res;
}

double satellite::math::first_row_moment ( const std::vector< std::pair<double, unsigned long> >& x, unsigned short degree ) {
  return satellite::math::moment(x, 0, 1);
};

double satellite::math::central_moment ( const std::vector< std::pair<double, unsigned long> >& x, unsigned short degree ) {
  return satellite::math::moment(x, satellite::math::first_row_moment(x, 1), 2);
};

double satellite::math::cov ( const std::vector< std::pair<double, unsigned long> >& x, const std::vector< std::pair<double, unsigned long> >& y ) {
  if (!x.size() || !y.size()) return 0;
  if (x.size() != y.size()) return 0/0.0;

  double res, xy, err;
  xy = satellite::math::first_row_moment(x, 1) * satellite::math::first_row_moment(y, 1);
  res = err = 0;
  std::vector< std::pair<double, unsigned long> >::const_iterator it_x, it_y;
  for (it_x = x.begin(), it_y = y.begin(); it_x != x.end(); ++it_x, ++it_y) {
    double buff, dx;

    dx = (it_x->second)*(it_y->second) / x.size() - err;
    buff = res + dx;
    err = (buff - res) - dx;
    res = buff;
  }

  res -= xy;

  return res;

};

size_t satellite::math::threshold_Otsu ( const std::vector< std::pair<double, unsigned long> >& x ) {
  // Введем два вспомогательных числа:
  long m = 0; // m - сумма высот всех бинов, домноженных на положение их середины
  long n = 0; // n - сумма высот всех бинов
  for (size_t t = 0; t < x.size(); ++t) {
    m += t * x[t].second;//temp
    n += x[t].second;//temp1
  }

  double maxSigma = -1; // Максимальное значение межклассовой дисперсии
  size_t threshold = 0; // Порог, соответствующий maxSigma

  long alpha = 0; // Сумма высот всех бинов для класса 1
  long beta = 0; // Сумма высот всех бинов для класса 1, домноженных на положение их середины

  for (size_t t = 0; t < x.size()-1; t++) {
    double w1, a, sigma;

    alpha += t* x[t].second;
    beta += x[t].second;

    w1 = (double)beta / n;
    a = (double)alpha / beta - (m - alpha) / (n - beta);
    sigma = w1 * (1 - w1) * a * a;

    if (sigma > maxSigma) {
      maxSigma = sigma;
      threshold = t;
    }
  }

  return threshold;
}
