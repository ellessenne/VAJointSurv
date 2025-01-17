// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "VAJointSurv_types.h"
#include <RcppArmadillo.h>
#include <RcppEigen.h>
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// expansion_object
SEXP expansion_object(List dat);
RcppExport SEXP _VAJointSurv_expansion_object(SEXP datSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< List >::type dat(datSEXP);
    rcpp_result_gen = Rcpp::wrap(expansion_object(dat));
    return rcpp_result_gen;
END_RCPP
}
// eval_expansion
NumericMatrix eval_expansion(SEXP ptr, NumericVector const x, int const ders, double lower_limit);
RcppExport SEXP _VAJointSurv_eval_expansion(SEXP ptrSEXP, SEXP xSEXP, SEXP dersSEXP, SEXP lower_limitSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< NumericVector const >::type x(xSEXP);
    Rcpp::traits::input_parameter< int const >::type ders(dersSEXP);
    Rcpp::traits::input_parameter< double >::type lower_limit(lower_limitSEXP);
    rcpp_result_gen = Rcpp::wrap(eval_expansion(ptr, x, ders, lower_limit));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_ptr
SEXP joint_ms_ptr(List markers, List survival_terms, unsigned const max_threads, List delayed_terms);
RcppExport SEXP _VAJointSurv_joint_ms_ptr(SEXP markersSEXP, SEXP survival_termsSEXP, SEXP max_threadsSEXP, SEXP delayed_termsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< List >::type markers(markersSEXP);
    Rcpp::traits::input_parameter< List >::type survival_terms(survival_termsSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type max_threads(max_threadsSEXP);
    Rcpp::traits::input_parameter< List >::type delayed_terms(delayed_termsSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_ptr(markers, survival_terms, max_threads, delayed_terms));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_n_terms
List joint_ms_n_terms(SEXP ptr);
RcppExport SEXP _VAJointSurv_joint_ms_n_terms(SEXP ptrSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_n_terms(ptr));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_eval_lb
double joint_ms_eval_lb(NumericVector val, SEXP ptr, unsigned const n_threads, List quad_rule, bool const cache_expansions, List gh_quad_rule);
RcppExport SEXP _VAJointSurv_joint_ms_eval_lb(SEXP valSEXP, SEXP ptrSEXP, SEXP n_threadsSEXP, SEXP quad_ruleSEXP, SEXP cache_expansionsSEXP, SEXP gh_quad_ruleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< NumericVector >::type val(valSEXP);
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type n_threads(n_threadsSEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< bool const >::type cache_expansions(cache_expansionsSEXP);
    Rcpp::traits::input_parameter< List >::type gh_quad_rule(gh_quad_ruleSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_eval_lb(val, ptr, n_threads, quad_rule, cache_expansions, gh_quad_rule));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_eval_lb_gr
NumericVector joint_ms_eval_lb_gr(NumericVector val, SEXP ptr, unsigned const n_threads, List quad_rule, bool const cache_expansions, List gh_quad_rule);
RcppExport SEXP _VAJointSurv_joint_ms_eval_lb_gr(SEXP valSEXP, SEXP ptrSEXP, SEXP n_threadsSEXP, SEXP quad_ruleSEXP, SEXP cache_expansionsSEXP, SEXP gh_quad_ruleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< NumericVector >::type val(valSEXP);
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type n_threads(n_threadsSEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< bool const >::type cache_expansions(cache_expansionsSEXP);
    Rcpp::traits::input_parameter< List >::type gh_quad_rule(gh_quad_ruleSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_eval_lb_gr(val, ptr, n_threads, quad_rule, cache_expansions, gh_quad_rule));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_hess
Eigen::SparseMatrix<double> joint_ms_hess(NumericVector val, SEXP ptr, List quad_rule, bool const cache_expansions, double const eps, double const scale, double const tol, unsigned const order, List gh_quad_rule);
RcppExport SEXP _VAJointSurv_joint_ms_hess(SEXP valSEXP, SEXP ptrSEXP, SEXP quad_ruleSEXP, SEXP cache_expansionsSEXP, SEXP epsSEXP, SEXP scaleSEXP, SEXP tolSEXP, SEXP orderSEXP, SEXP gh_quad_ruleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< NumericVector >::type val(valSEXP);
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< bool const >::type cache_expansions(cache_expansionsSEXP);
    Rcpp::traits::input_parameter< double const >::type eps(epsSEXP);
    Rcpp::traits::input_parameter< double const >::type scale(scaleSEXP);
    Rcpp::traits::input_parameter< double const >::type tol(tolSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type order(orderSEXP);
    Rcpp::traits::input_parameter< List >::type gh_quad_rule(gh_quad_ruleSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_hess(val, ptr, quad_rule, cache_expansions, eps, scale, tol, order, gh_quad_rule));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_parameter_names
List joint_ms_parameter_names(SEXP ptr);
RcppExport SEXP _VAJointSurv_joint_ms_parameter_names(SEXP ptrSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_parameter_names(ptr));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_parameter_indices
List joint_ms_parameter_indices(SEXP ptr);
RcppExport SEXP _VAJointSurv_joint_ms_parameter_indices(SEXP ptrSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_parameter_indices(ptr));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_n_params
int joint_ms_n_params(SEXP ptr);
RcppExport SEXP _VAJointSurv_joint_ms_n_params(SEXP ptrSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_n_params(ptr));
    return rcpp_result_gen;
END_RCPP
}
// opt_priv
NumericVector opt_priv(NumericVector val, SEXP ptr, double const rel_eps, unsigned const max_it, unsigned const n_threads, double const c1, double const c2, List quad_rule, bool const cache_expansions, double const gr_tol, List gh_quad_rule);
RcppExport SEXP _VAJointSurv_opt_priv(SEXP valSEXP, SEXP ptrSEXP, SEXP rel_epsSEXP, SEXP max_itSEXP, SEXP n_threadsSEXP, SEXP c1SEXP, SEXP c2SEXP, SEXP quad_ruleSEXP, SEXP cache_expansionsSEXP, SEXP gr_tolSEXP, SEXP gh_quad_ruleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< NumericVector >::type val(valSEXP);
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< double const >::type rel_eps(rel_epsSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type max_it(max_itSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type n_threads(n_threadsSEXP);
    Rcpp::traits::input_parameter< double const >::type c1(c1SEXP);
    Rcpp::traits::input_parameter< double const >::type c2(c2SEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< bool const >::type cache_expansions(cache_expansionsSEXP);
    Rcpp::traits::input_parameter< double const >::type gr_tol(gr_tolSEXP);
    Rcpp::traits::input_parameter< List >::type gh_quad_rule(gh_quad_ruleSEXP);
    rcpp_result_gen = Rcpp::wrap(opt_priv(val, ptr, rel_eps, max_it, n_threads, c1, c2, quad_rule, cache_expansions, gr_tol, gh_quad_rule));
    return rcpp_result_gen;
END_RCPP
}
// joint_ms_opt_lb
List joint_ms_opt_lb(NumericVector val, SEXP ptr, double const rel_eps, unsigned const max_it, unsigned const n_threads, double const c1, double const c2, bool const use_bfgs, unsigned const trace, double const cg_tol, bool const strong_wolfe, size_t const max_cg, unsigned const pre_method, List quad_rule, Rcpp::IntegerVector mask, bool const cache_expansions, bool const only_markers, double const gr_tol, List gh_quad_rule);
RcppExport SEXP _VAJointSurv_joint_ms_opt_lb(SEXP valSEXP, SEXP ptrSEXP, SEXP rel_epsSEXP, SEXP max_itSEXP, SEXP n_threadsSEXP, SEXP c1SEXP, SEXP c2SEXP, SEXP use_bfgsSEXP, SEXP traceSEXP, SEXP cg_tolSEXP, SEXP strong_wolfeSEXP, SEXP max_cgSEXP, SEXP pre_methodSEXP, SEXP quad_ruleSEXP, SEXP maskSEXP, SEXP cache_expansionsSEXP, SEXP only_markersSEXP, SEXP gr_tolSEXP, SEXP gh_quad_ruleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< NumericVector >::type val(valSEXP);
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< double const >::type rel_eps(rel_epsSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type max_it(max_itSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type n_threads(n_threadsSEXP);
    Rcpp::traits::input_parameter< double const >::type c1(c1SEXP);
    Rcpp::traits::input_parameter< double const >::type c2(c2SEXP);
    Rcpp::traits::input_parameter< bool const >::type use_bfgs(use_bfgsSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type trace(traceSEXP);
    Rcpp::traits::input_parameter< double const >::type cg_tol(cg_tolSEXP);
    Rcpp::traits::input_parameter< bool const >::type strong_wolfe(strong_wolfeSEXP);
    Rcpp::traits::input_parameter< size_t const >::type max_cg(max_cgSEXP);
    Rcpp::traits::input_parameter< unsigned const >::type pre_method(pre_methodSEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerVector >::type mask(maskSEXP);
    Rcpp::traits::input_parameter< bool const >::type cache_expansions(cache_expansionsSEXP);
    Rcpp::traits::input_parameter< bool const >::type only_markers(only_markersSEXP);
    Rcpp::traits::input_parameter< double const >::type gr_tol(gr_tolSEXP);
    Rcpp::traits::input_parameter< List >::type gh_quad_rule(gh_quad_ruleSEXP);
    rcpp_result_gen = Rcpp::wrap(joint_ms_opt_lb(val, ptr, rel_eps, max_it, n_threads, c1, c2, use_bfgs, trace, cg_tol, strong_wolfe, max_cg, pre_method, quad_rule, mask, cache_expansions, only_markers, gr_tol, gh_quad_rule));
    return rcpp_result_gen;
END_RCPP
}
// ph_ll
List ph_ll(List time_fixef, NumericMatrix Z, NumericMatrix surv, bool const with_frailty);
RcppExport SEXP _VAJointSurv_ph_ll(SEXP time_fixefSEXP, SEXP ZSEXP, SEXP survSEXP, SEXP with_frailtySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< List >::type time_fixef(time_fixefSEXP);
    Rcpp::traits::input_parameter< NumericMatrix >::type Z(ZSEXP);
    Rcpp::traits::input_parameter< NumericMatrix >::type surv(survSEXP);
    Rcpp::traits::input_parameter< bool const >::type with_frailty(with_frailtySEXP);
    rcpp_result_gen = Rcpp::wrap(ph_ll(time_fixef, Z, surv, with_frailty));
    return rcpp_result_gen;
END_RCPP
}
// ph_eval
double ph_eval(SEXP ptr, NumericVector par, List quad_rule, double const va_var);
RcppExport SEXP _VAJointSurv_ph_eval(SEXP ptrSEXP, SEXP parSEXP, SEXP quad_ruleSEXP, SEXP va_varSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type par(parSEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< double const >::type va_var(va_varSEXP);
    rcpp_result_gen = Rcpp::wrap(ph_eval(ptr, par, quad_rule, va_var));
    return rcpp_result_gen;
END_RCPP
}
// ph_grad
NumericVector ph_grad(SEXP ptr, NumericVector par, List quad_rule, double const va_var);
RcppExport SEXP _VAJointSurv_ph_grad(SEXP ptrSEXP, SEXP parSEXP, SEXP quad_ruleSEXP, SEXP va_varSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::traits::input_parameter< SEXP >::type ptr(ptrSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type par(parSEXP);
    Rcpp::traits::input_parameter< List >::type quad_rule(quad_ruleSEXP);
    Rcpp::traits::input_parameter< double const >::type va_var(va_varSEXP);
    rcpp_result_gen = Rcpp::wrap(ph_grad(ptr, par, quad_rule, va_var));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP run_testthat_tests(SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"_VAJointSurv_expansion_object", (DL_FUNC) &_VAJointSurv_expansion_object, 1},
    {"_VAJointSurv_eval_expansion", (DL_FUNC) &_VAJointSurv_eval_expansion, 4},
    {"_VAJointSurv_joint_ms_ptr", (DL_FUNC) &_VAJointSurv_joint_ms_ptr, 4},
    {"_VAJointSurv_joint_ms_n_terms", (DL_FUNC) &_VAJointSurv_joint_ms_n_terms, 1},
    {"_VAJointSurv_joint_ms_eval_lb", (DL_FUNC) &_VAJointSurv_joint_ms_eval_lb, 6},
    {"_VAJointSurv_joint_ms_eval_lb_gr", (DL_FUNC) &_VAJointSurv_joint_ms_eval_lb_gr, 6},
    {"_VAJointSurv_joint_ms_hess", (DL_FUNC) &_VAJointSurv_joint_ms_hess, 9},
    {"_VAJointSurv_joint_ms_parameter_names", (DL_FUNC) &_VAJointSurv_joint_ms_parameter_names, 1},
    {"_VAJointSurv_joint_ms_parameter_indices", (DL_FUNC) &_VAJointSurv_joint_ms_parameter_indices, 1},
    {"_VAJointSurv_joint_ms_n_params", (DL_FUNC) &_VAJointSurv_joint_ms_n_params, 1},
    {"_VAJointSurv_opt_priv", (DL_FUNC) &_VAJointSurv_opt_priv, 11},
    {"_VAJointSurv_joint_ms_opt_lb", (DL_FUNC) &_VAJointSurv_joint_ms_opt_lb, 19},
    {"_VAJointSurv_ph_ll", (DL_FUNC) &_VAJointSurv_ph_ll, 4},
    {"_VAJointSurv_ph_eval", (DL_FUNC) &_VAJointSurv_ph_eval, 4},
    {"_VAJointSurv_ph_grad", (DL_FUNC) &_VAJointSurv_ph_grad, 4},
    {"run_testthat_tests", (DL_FUNC) &run_testthat_tests, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_VAJointSurv(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
