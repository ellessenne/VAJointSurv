#include "integrand-expected-survival.h"
#include "ghq-lp-utils.h"

namespace ghqCpp {

template<bool comp_grad>
expected_survival_term<comp_grad>::expected_survival_term
  (arma::vec const &eta, arma::vec const &weights, arma::mat const &M,
   arma::mat const &Sigma):
  eta{eta}, weights{weights}, Sigma_chol(arma::chol(Sigma)),
  M_Sigma_chol_t{M * Sigma_chol.t()} {
    if(eta.n_elem != weights.n_elem)
      throw std::invalid_argument("eta.n_elem != weights.n_elem");
    else if(eta.n_elem != M_Sigma_chol_t.n_rows)
      throw std::invalid_argument("eta.n_elem != M_Sigma_chol_t.n_rows");
  }

template<bool comp_grad>
void expected_survival_term<comp_grad>::eval
  (double const *points, size_t const n_points, double * __restrict__ outs,
   simple_mem_stack<double> &mem) const {
  size_t const n_lps = M_Sigma_chol_t.n_rows;
  double * const __restrict__ lps{comp_grad ? outs + n_points
                                            : mem.get(n_lps * n_points)};
  auto mem_mark = mem.set_mark_raii();

  // add the offset
  for(size_t i = 0; i < n_lps; ++i)
    std::fill(lps + i * n_points, lps + (i + 1) * n_points, eta[i]);

  // add the terms from the random effects
  {
    double const * m{M_Sigma_chol_t.begin()};
    for(size_t k = 0; k < n_vars(); ++k)
      for(size_t i = 0; i < n_lps; ++i)
        for(size_t j = 0; j < n_points; ++j)
          lps[j + i * n_points] += m[i + k * n_lps] * points[j + k * n_points];
  }

  for(size_t ij = 0; ij < n_points * n_lps; ++ij)
    lps[ij] = std::exp(lps[ij]);

  // weight the points
  for(size_t i = 0; i < n_lps; ++i)
    for(size_t j = 0; j < n_points; ++j)
      lps[j + i * n_points] *= -weights[i];

  // compute the weighted sum
  std::fill(outs, outs + n_points, 0);
  for(size_t i = 0; i < n_lps; ++i)
    for(size_t j = 0; j < n_points; ++j)
      outs[j] += lps[j + i * n_points];
  for(size_t j = 0; j < n_points; ++j)
    outs[j] = std::exp(outs[j]);

  if constexpr(comp_grad){
    double * const __restrict__ d_lps{outs + n_points};
    double * const __restrict__ us{mem.get(n_points * n_vars())};

    // finish the computation of the derivatives w.r.t. each linear predictor
    for(size_t i = 0; i < n_lps; ++i)
      for(size_t j = 0; j < n_points; ++j)
        d_lps[j + i * n_points] *= outs[j];

    // the derivatives w.r.t. eta are already set
    outs += n_points;

    // compute the derivatives w.r.t. M
    outs += n_points * n_lps;

    std::copy(points, points + n_points * n_vars(), us);
    {
      int const m = n_points, n = n_vars();
      constexpr double const alpha{1};
      constexpr char const c_R{'R'}, c_U{'U'}, c_N{'N'};
      F77_CALL(dtrmm)
        (&c_R, &c_U, &c_N, &c_N, &m, &n, &alpha, Sigma_chol.memptr(), &n,
         us, &m, 1, 1, 1, 1);
    }

    for(size_t k = 0; k < n_vars(); ++k)
      for(size_t i = 0; i < n_lps; ++i)
        for(size_t j = 0; j < n_points; ++j)
          outs[j + i * n_points + k * n_points * n_lps] =
            d_lps[j + i * n_points] * us[j + k * n_points];

  }
}

template<bool comp_grad>
double expected_survival_term<comp_grad>::log_integrand
  (double const *point, simple_mem_stack<double> &mem) const {
  size_t const n_lps = M_Sigma_chol_t.n_rows;
  double * const __restrict__ lp{mem.get(n_lps)};
  std::copy(eta.begin(), eta.end(), lp);

  {
    double const * m{M_Sigma_chol_t.begin()};
    for(size_t k = 0; k < n_vars(); ++k)
      for(size_t i = 0; i < n_lps; ++i)
        lp[i] += m[i + k * n_lps] * point[k];
  }

  double out{};
  for(size_t i = 0; i < n_lps; ++i)
    out -= weights[i] * std::exp(lp[i]);

  return out;
}

template<bool comp_grad>
double expected_survival_term<comp_grad>::log_integrand_grad
  (double const *point, double * __restrict__ grad,
   simple_mem_stack<double> &mem) const {
  size_t const n_lps = M_Sigma_chol_t.n_rows;
  double * const __restrict__ lp{mem.get(n_lps)};
  std::copy(eta.begin(), eta.end(), lp);

  {
    double const * m{M_Sigma_chol_t.begin()};
    for(size_t k = 0; k < n_vars(); ++k)
      for(size_t i = 0; i < n_lps; ++i)
        lp[i] += m[i + k * n_lps] * point[k];
  }

  double out{};
  for(size_t i = 0; i < n_lps; ++i){
    lp[i] = -weights[i] * std::exp(lp[i]); // the derivative w.r.t. lp
    out += lp[i];
  }

  std::fill(grad, grad + n_vars(), 0);
  double const * m{M_Sigma_chol_t.begin()};
  for(size_t k = 0; k < n_vars(); ++k)
    for(size_t i = 0; i < n_lps; ++i)
      grad[k] += m[i + k * n_lps] * lp[i];

  return out;
}

template<bool comp_grad>
void expected_survival_term<comp_grad>::log_integrand_hess
  (double const *point, double *hess, simple_mem_stack<double> &mem) const {
  size_t const n_lps = M_Sigma_chol_t.n_rows;
  double * const __restrict__ lp{mem.get(n_lps)};
  std::copy(eta.begin(), eta.end(), lp);

  {
    double const * m{M_Sigma_chol_t.begin()};
    for(size_t k = 0; k < n_vars(); ++k)
      for(size_t i = 0; i < n_lps; ++i)
        lp[i] += m[i + k * n_lps] * point[k];
  }

  double out{};
  for(size_t i = 0; i < n_lps; ++i){
    lp[i] = -weights[i] * std::exp(lp[i]); // the derivative w.r.t. lp
    out += lp[i];
  }

  // TODO: do this smarter
  arma::mat H(hess, n_vars(), n_vars(), false);
  arma::mat dum(n_lps, n_lps, arma::fill::zeros);
  for(arma::uword i = 0; i < n_lps; ++i)
    dum(i, i) = lp[i];

  H = M_Sigma_chol_t.t() * dum * M_Sigma_chol_t;
}

template class expected_survival_term<false>;
template class expected_survival_term<true>;

} // namespace ghqCpp
