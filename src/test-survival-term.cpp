#include "survival-term.h"
#include "testthat-wrapper.h"
#include "wmem.h"
#include <iterator>

using std::begin;
using std::end;
using cfaad::Number;

namespace {
/*
  Legendre quadrature rule. From
    library(gaussquad)
    dat <- legendre.quadrature.rules(100)[[100L]]
    dput(dat$x / 2 + .5)
    dput(dat$w / 2)
 */
constexpr vajoint_uint n_nodes{100};
  constexpr double ns[n_nodes] {0.999856863386721, 0.999245975319798, 0.998147567366563, 0.996562468518722, 0.994492197621496, 0.991938770353028, 0.988904679243459, 0.985392887881853, 0.981406827127908, 0.976950391462746, 0.972027935068128, 0.96664426752154, 0.960804649072667, 0.954514785491265, 0.947780822485363, 0.940609339692509, 0.933007344248582, 0.924982263939796, 0.9165419399442, 0.907694619169588, 0.898448946195157, 0.888813954824748, 0.878799059259854, 0.86841404490101, 0.857669058786528, 0.846574599677901, 0.835141507801571, 0.823380954257065, 0.811304430101854, 0.798923735123589, 0.786250966310691, 0.773298506032547, 0.760079009940882, 0.746605394604095, 0.732890824886679, 0.718948701086016, 0.704792645839151, 0.690436490812315, 0.675894263186211, 0.661180171950264, 0.646308594019236, 0.631294060185752, 0.616151240922487, 0.600894932047868, 0.585540040269302, 0.570101568618057, 0.55459460179003, 0.539034291406718, 0.523435841210796, 0.507814492210772, 0.492185507789228, 0.476564158789204, 0.460965708593282, 0.445405398209969, 0.429898431381943, 0.414459959730698, 0.399105067952132, 0.383848759077513, 0.368705939814248, 0.353691405980764, 0.338819828049735, 0.324105736813789, 0.309563509187685, 0.295207354160849, 0.281051298913984, 0.267109175113321, 0.253394605395905, 0.239920990059119, 0.226701493967453, 0.213749033689309, 0.201076264876411, 0.188695569898146, 0.176619045742935, 0.16485849219843, 0.153425400322099, 0.142330941213472, 0.13158595509899, 0.121200940740146, 0.111186045175252, 0.101551053804843, 0.0923053808304122, 0.0834580600557998, 0.0750177360602048, 0.0669926557514177, 0.059390660307491, 0.0522191775146367, 0.045485214508735, 0.0391953509273331, 0.0333557324784605, 0.0279720649318723, 0.0230496085372542, 0.0185931728720925, 0.0146071121181469, 0.0110953207565408, 0.00806122964697142, 0.00550780237850412, 0.00343753148127823, 0.0018524326334376, 0.000754024680202081, 0.000143136613279637},
                   ws[n_nodes] {0.00036731724525283, 0.00085469632675904, 0.00134196268577676, 0.0018279806006632, 0.00231222503171107, 0.00279421400193276, 0.00327347422542265, 0.0037495366277323, 0.00422193573483449, 0.00469020982684724, 0.00515390128743443, 0.00561255701159295, 0.00606572883148976, 0.00651297394648573, 0.00695385535185937, 0.00738794226372056, 0.00781481053877309, 0.00823404308807255, 0.00864523028416182, 0.00904797036106383, 0.0094418698066875, 0.00982654374721765, 0.0102016163231046, 0.010566721056264, 0.0109215012081237, 0.0112656101281682, 0.0115987115926271, 0.0119204801329841, 0.0122306013539787, 0.0125287722407897, 0.0128147014551038, 0.0130881096197728, 0.0133487295917854, 0.0135963067232884, 0.0138305991103962, 0.0140513778295507, 0.0142584271611973, 0.0144515448005625, 0.0146305420553188, 0.0147952440299562, 0.0149454897966664, 0.0150811325525845, 0.0152020397632271, 0.0153080932919901, 0.0153991895155767, 0.0154752394252457, 0.0155361687137837, 0.015581917848105, 0.0156124421274248, 0.0156277117269315, 0.0156277117269314, 0.0156124421274247, 0.0155819178481047, 0.0155361687137839, 0.0154752394252455, 0.0153991895155768, 0.0153080932919904, 0.0152020397632275, 0.0150811325525847, 0.0149454897966667, 0.0147952440299567, 0.014630542055319, 0.0144515448005628, 0.0142584271611974, 0.0140513778295502, 0.0138305991103963, 0.013596306723289, 0.0133487295917851, 0.0130881096197728, 0.0128147014551039, 0.0125287722407895, 0.0122306013539787, 0.0119204801329843, 0.011598711592627, 0.0112656101281685, 0.0109215012081232, 0.0105667210562641, 0.0102016163231051, 0.00982654374721767, 0.0094418698066878, 0.00904797036106456, 0.00864523028416149, 0.00823404308807257, 0.00781481053877307, 0.00738794226372049, 0.00695385535185915, 0.00651297394648541, 0.00606572883148956, 0.00561255701159312, 0.00515390128743413, 0.00469020982684725, 0.0042219357348343, 0.00374953662773162, 0.00327347422542264, 0.00279421400193271, 0.00231222503171171, 0.00182798060066296, 0.00134196268577661, 0.000854696326758952, 0.000367317245252787};

} // namespace

context("expected_cum_hazzard is correct") {
  test_that("expected_cum_hazzard gives the correct result"){
    /*
     raw_poly <- function(x, degree, intercept){
     if(intercept)
     drop(outer(x, 0:degree, `^`))
     else
     drop(outer(x, 1:degree, `^`))
     }

# parameters
     Z <- c(1, -.5, .33)
     delta <- c(.1, .2, -.3)
     g <- function(x) raw_poly(x, 2, FALSE)
     omega <- c(.2, -.33)
     alpha <- c(.1, .4, -.2)
     ms <- list(function(x) raw_poly(x, 1, TRUE),
     function(x) raw_poly(x, 2, TRUE),
     function(x) raw_poly(x, 1, TRUE))
     zeta <- c(-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7)
     set.seed(1)
     dput(Psi <- drop(round(rWishart(1, 16, diag(.025, 8)), 3)))
     stopifnot(all(eigen(Psi)$value > 0))

     f <- function(args, lb, ub){
     get_next <- function(n){
     out <- head(args, n)
     args <<- tail(args, -n)
     out
     }
     delta <- get_next(length(delta))
     omega <- get_next(length(omega))
     alpha <- get_next(length(alpha))
     zeta <- get_next(length(zeta))
     Psi <- matrix(args, NROW(Psi))

     integrand <- function(x){
     M <- matrix(0, length(zeta), length(ms) + 1)
     offset <- 0L
     for(i in seq_along(ms)){
     z <- ms[[i]](x)
     M[offset + seq_along(z), i] <- z
     offset <- offset + length(z)
     }
     M[length(zeta), length(ms) + 1L] <- 1

     M_alpha <- drop(M %*% c(alpha, 1))

     exp(delta %*% Z + g(x) %*% omega + M_alpha %*% zeta +
     M_alpha %*% Psi %*% M_alpha / 2)
     }

     integrate(Vectorize(integrand), lb, ub, rel.tol = 1e-10)$value
     }

     dput(f(c(delta, omega, alpha, zeta, Psi), 0, 2))
     dput(numDeriv::grad(f, c(delta, omega, alpha, zeta, Psi), lb = 0, ub = 2))

     dput(f(c(delta, omega, alpha, zeta, Psi), 1, 3))
     dput(numDeriv::grad(f, c(delta, omega, alpha, zeta, Psi), lb = 1, ub = 3))
     */
    constexpr double z[] {1, -.5, .33},
                 delta[] {.1, .2, -.3},
                 alpha[] {.1, .4, -.2},
                 omega[] {.2, -.33},
                  zeta[] {-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7 },
                   Psi[] {0.294, 0.109, -0.132, 0.049, -0.053, 0.037, -0.005, -0.009, 0.109, 0.588, -0.158, -0.017, -0.279, -0.131, 0.057, 0.042, -0.132, -0.158, 0.461, 0.132, 0.185, -0.01, 0.096, -0.01, 0.049, -0.017, 0.132, 0.333, 0.047, 0.038, -0.02, -0.119, -0.053, -0.279, 0.185, 0.047, 0.487, 0.067, -0.111, -0.057, 0.037, -0.131, -0.01, 0.038, 0.067, 0.296, -0.029, -0.058, -0.005, 0.057, 0.096, -0.02, -0.111, -0.029, 0.408, 0.035, -0.009, 0.042, -0.01, -0.119, -0.057, -0.058, 0.035, 0.237},
                   lb1   {0},
                   ub1   {2},
                   lb2   {1},
                   ub2   {3},
             true_val1   {3.66100103931602},
             true_val2   {4.19535676757197},
                   gr1[] {3.66100103883071, -1.83050051941535, 1.20813034296761, 3.42918046430273, 4.4106573267205, -1.6246994498572, 3.16498025440435, -0.687486072574645, 0.366100103838971, 0.342918046455012, 1.46440041545206, 1.371672186477, 1.76426293173328, -0.732200208058255, -0.685836092990732, 3.66100103939786, 0.0183050050597476, 0.0171459024689582, 0.0732200207545565, 0.068583609268588, 0.0882131467175713, -0.0366100092407855, -0.0342918151909225, 0.183050051180727, 0.0171459024689582, 0.0220532865467495, 0.0685836090128747, 0.0882131460840674, 0.129601047313787, -0.034291804711143, -0.0441065736611287, 0.17145902296696, 0.0732200207545565, 0.0685836090128747, 0.292880083117788, 0.274334437573024, 0.35285258611587, -0.146440042696389, -0.137167218198237, 0.732200208441336, 0.068583609268588, 0.0882131460840674, 0.274334437573024, 0.352852586225271, 0.518404189825079, -0.137167217730308, -0.176426293334296, 0.685836092756295, 0.0882131467175713, 0.129601047313784, 0.35285258613863, 0.518404189825079, 0.819070183872609, -0.176426292755572, -0.25920209489517, 0.882131465487062, -0.0366100092407855, -0.034291804711143, -0.146440042696389, -0.137167217730308, -0.176426292755572, 0.0732200207972134, 0.0685836092897979, -0.366100104167798, -0.0342918151909225, -0.0441065736611012, -0.137167218198237, -0.176426293334296, -0.259202094626022, 0.0685836084048065, 0.0882131465370985, -0.342918047199904, 0.183050051180727, 0.17145902296696, 0.732200208441336, 0.685836092756295, 0.882131465487062, -0.366100104167798, -0.342918047933183, 1.83050051953017},
                   gr2[] {4.19535676741652, -2.09767838370826, 1.38446773339962, 9.08862977590842, 21.2931572851835, -7.31938925798106, 31.5115790008941, -3.93046899110043, 0.419535676520029, 0.908862977519579, 1.67814270705777, 3.63545190818828, 8.51726291308962, -0.839071353751239, -1.8177259550839, 4.19535676754135, 0.0209767839346527, 0.0454431488565312, 0.0839071352485557, 0.181772595700282, 0.425863145996658, -0.0419535673592843, -0.0908863164064654, 0.209767833938893, 0.0454431488565312, 0.106465786465339, 0.18177259551448, 0.425863144529527, 1.05488916587426, -0.0908862973834766, -0.212931573482166, 0.454431490473678, 0.0839071352505494, 0.18177259546119, 0.335628541527436, 0.727090381980296, 1.70345258301744, -0.167814276925219, -0.363545190903175, 0.839071354593133, 0.181772594652741, 0.425863144529527, 0.727090382369155, 1.70345258262788, 4.21955666360319, -0.363545192501694, -0.85172629128636, 1.81772595483077, 0.425863146801342, 1.0548891658752, 1.70345258325081, 4.21955666251104, 10.8606650093518, -0.851726291359716, -2.10977833201371, 4.25863145608917, -0.0419535675868855, -0.0908862973834766, -0.167814266633001, -0.363545192487844, -0.851726291607169, 0.083907135469042, 0.181772596328033, -0.419535676547273, -0.0908863044560012, -0.212931572581648, -0.363545190900434, -0.851726288706805, -2.10977833201132, 0.181772596327979, 0.425863145852749, -0.908862977753331, 0.209767834845172, 0.454431489251547, 0.839071354593133, 1.8177259552599, 4.2586314580333, -0.419535676547273, -0.908862977512679, 2.09767838381014};

    Number ad_delta[3],
           ad_alpha[3],
           ad_omega[2],
            ad_zeta[8],
             ad_Psi[64];

    joint_bases::orth_poly g{2, false};

    joint_bases::bases_vector bases_rng;
    // raw poly of degree x with an intercept
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(2, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));

    // we get the correct value
    std::vector<std::vector<int> > ders{{0}, {0}, {0}};
    survival::expected_cum_hazzard comp_obj(g, bases_rng, 3, ders, true);
    std::vector<double> expansions(comp_obj.cache_mem_per_node() * n_nodes);
    {
      auto req_mem = comp_obj.n_wmem();
      double res = comp_obj(
        {ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
        wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
        nullptr);

      expect_true(pass_rel_err(res, true_val1, 1e-6));

      // with cached expansions
      comp_obj.cache_expansions
        (lb1, ub1, expansions.data(), wmem::get_double_mem(req_mem[1]),
         {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         expansions.data());
      expect_true(pass_rel_err(res, true_val1, 1e-6));
    }
    {
      auto req_mem = comp_obj.n_wmem();
      double res = comp_obj(
        {ns, ws, n_nodes}, lb2, ub2, z, delta, omega, alpha, zeta, Psi,
        wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
        nullptr);

      expect_true(pass_rel_err(res, true_val2, 1e-6));

      // with cached expansions
      comp_obj.cache_expansions
        (lb2, ub2, expansions.data(), wmem::get_double_mem(req_mem[1]),
        {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         expansions.data());
      expect_true(pass_rel_err(res, true_val2, 1e-6));
    }

    auto rewind_n_convert = [&]{
      Number::tape->rewind();
      cfaad::convertCollection(begin(delta), end(delta), ad_delta);
      cfaad::convertCollection(begin(omega), end(omega), ad_omega);
      cfaad::convertCollection(begin(alpha), end(alpha), ad_alpha);
      cfaad::convertCollection(begin(zeta), end(zeta), ad_zeta);
      cfaad::convertCollection(begin(Psi), end(Psi), ad_Psi);
    };

    // we get the correct gradient
    {
      rewind_n_convert();
      auto req_mem = comp_obj.n_wmem();
      Number res = comp_obj(
        {ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
        ad_Psi, wmem::get_Number_mem(req_mem[0]),
        wmem::get_double_mem(req_mem[1]), nullptr);

      expect_true(pass_rel_err(res.value(), true_val1, 1e-6));
      res.propagateToStart();
      double const *g{gr1};

      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));

      // with cached memory
      rewind_n_convert();
      comp_obj.cache_expansions
        (lb1, ub1, expansions.data(), wmem::get_double_mem(req_mem[1]),
         {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
         ad_Psi, wmem::get_Number_mem(req_mem[0]),
         wmem::get_double_mem(req_mem[1]), expansions.data());

      expect_true(pass_rel_err(res.value(), true_val1, 1e-6));
      res.propagateToStart();

      g = gr1;
      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
    }
    {
      rewind_n_convert();

      auto req_mem = comp_obj.n_wmem();
      Number res = comp_obj(
        {ns, ws, n_nodes}, lb2, ub2, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
        ad_Psi, wmem::get_Number_mem(req_mem[0]),
        wmem::get_double_mem(req_mem[1]), nullptr);

      expect_true(pass_rel_err(res.value(), true_val2, 1e-6));
      res.propagateToStart();
      double const *g{gr2};

      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));

      // with cached memory
      rewind_n_convert();
      comp_obj.cache_expansions
        (lb2, ub2, expansions.data(), wmem::get_double_mem(req_mem[1]),
        {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb2, ub2, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
         ad_Psi, wmem::get_Number_mem(req_mem[0]),
         wmem::get_double_mem(req_mem[1]), expansions.data());

      expect_true(pass_rel_err(res.value(), true_val2, 1e-6));
      res.propagateToStart();

      g = gr2;
      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
    }

    // clean-up
    wmem::clear_all();
  }

  test_that("expected_cum_hazzard gives the correct result without frailty"){
    /*
     raw_poly <- function(x, degree, intercept){
     if(intercept)
     drop(outer(x, 0:degree, `^`))
     else
     drop(outer(x, 1:degree, `^`))
     }

# parameters
     Z <- c(1, -.5, .33)
     delta <- c(.1, .2, -.3)
     g <- function(x) raw_poly(x, 2, FALSE)
     omega <- c(.2, -.33)
     alpha <- c(.1, .4, -.2)
     ms <- list(function(x) raw_poly(x, 1, TRUE),
     function(x) raw_poly(x, 2, TRUE),
     function(x) raw_poly(x, 1, TRUE))
     zeta <- c(-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126)
     set.seed(1)
     dput(Psi <- drop(round(rWishart(1, 16, diag(.025, 7)), 3)))
     stopifnot(all(eigen(Psi)$value > 0))

     f <- function(args, lb, ub){
     get_next <- function(n){
     out <- head(args, n)
     args <<- tail(args, -n)
     out
     }
     delta <- get_next(length(delta))
     omega <- get_next(length(omega))
     alpha <- get_next(length(alpha))
     zeta <- get_next(length(zeta))
     Psi <- matrix(args, NROW(Psi))

     integrand <- function(x){
     M <- matrix(0, length(zeta), length(ms))
     offset <- 0L
     for(i in seq_along(ms)){
     z <- ms[[i]](x)
     M[offset + seq_along(z), i] <- z
     offset <- offset + length(z)
     }

     M_alpha <- drop(M %*% alpha)

     exp(delta %*% Z + g(x) %*% omega + M_alpha %*% zeta +
     M_alpha %*% Psi %*% M_alpha / 2)
     }

     integrate(Vectorize(integrand), lb, ub, rel.tol = 1e-10)$value
     }

     dput(f(c(delta, omega, alpha, zeta, Psi), 0, 2))
     dput(numDeriv::grad(f, c(delta, omega, alpha, zeta, Psi), lb = 0, ub = 2))

     dput(f(c(delta, omega, alpha, zeta, Psi), 1, 3))
     dput(numDeriv::grad(f, c(delta, omega, alpha, zeta, Psi), lb = 1, ub = 3))
     */
    constexpr double z[] {1, -.5, .33},
                 delta[] {.1, .2, -.3},
                 alpha[] {.1, .4, -.2},
                 omega[] {.2, -.33},
                  zeta[] {-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126},
                   Psi[] {0.294, 0.109, -0.132, 0.049, -0.053, 0.037, -0.005, 0.109, 0.588, -0.158, -0.017, -0.279, -0.131, 0.057, -0.132, -0.158, 0.461, 0.132, 0.185, -0.01, 0.096, 0.049, -0.017, 0.132, 0.333, 0.047, 0.038, -0.02, -0.053, -0.279, 0.185, 0.047, 0.487, 0.067, -0.111, 0.037, -0.131, -0.01, 0.038, 0.067, 0.296, -0.029, -0.005, 0.057, 0.096, -0.02, -0.111, -0.029, 0.408},
                   lb1   {0},
                   ub1   {2},
                   lb2   {1},
                   ub2   {3},
             true_val1   {1.73105231299269},
             true_val2   {2.3109840405025},
                   gr1[] {1.73105231304629, -0.865526156523145, 0.571247263306151, 1.67584880157848, 2.19632972414468, -0.850927744111685, 1.93017169982009, -0.300750091316611, 0.17310523130411, 0.16758488021254, 0.692420924871468, 0.670339520197242, 0.878531889847776, -0.346210462562529, -0.335169760085916, 0.00865526157080918, 0.00837924408487893, 0.0346210461511111, 0.033516976304557, 0.0439265942765341, -0.0173105228747201, -0.0167584798233712, 0.00837924408487893, 0.0109816486250253, 0.0335169757997189, 0.0439265947731833, 0.0653148037504997, -0.0167584879673576, -0.0219632973017557, 0.0346210461511111, 0.033516975962154, 0.138484185081858, 0.134067904006304, 0.175706378008521, -0.0692420909871097, -0.0670339520921989, 0.0335169763475226, 0.0439265947731833, 0.134067904006298, 0.175706377877782, 0.261259214964611, -0.0670339519582168, -0.0878531892634446, 0.0439265942765341, 0.0653148037504997, 0.17570637799714, 0.261259214691582, 0.416212065151412, -0.0878531893843168, -0.130629607312473, -0.0173105228747201, -0.0167584879673576, -0.0692420909871097, -0.0670339519599481, -0.0878531892251933, 0.0346210463200971, 0.0335169757955579, -0.0167584798233712, -0.0219632975268851, -0.0670339520921989, -0.0878531893687102, -0.130629607312473, 0.0335169757955579, 0.0439265944806003},
                   gr2[] {2.3109840406161, -1.15549202030805, 0.762624733363528, 5.13260272940732, 12.2568257304768, -4.42747776636943, 19.8490538121894, -2.32280660662384, 0.231098404057677, 0.513260273014546, 0.924393616432615, 2.05304109436491, 4.90273029229812, -0.462196807817001, -1.02652054588619, 0.011554920225038, 0.0256630137300429, 0.0462196808742369, 0.102652054742906, 0.245136515009671, -0.0231098400671313, -0.0513260320969999, 0.0256630134945864, 0.0612841285833005, 0.102652054510316, 0.245136514238331, 0.615776956898666, -0.0513260274278542, -0.122568257224105, 0.0462196808742369, 0.102652054510316, 0.184878723179027, 0.410608218434666, 0.980546058358326, -0.092439361054317, -0.205304109267329, 0.102652054742906, 0.245136514238331, 0.410608218629096, 0.980546058391579, 2.46310782795637, -0.205304109037533, -0.490273030568781, 0.2451365150916, 0.615776956990654, 0.98054605863649, 2.46310782804596, 6.4045779750437, -0.490273029653842, -1.23155391409913, -0.0231098399533307, -0.0513260274278542, -0.0924393610674752, -0.205304109259124, -0.490273029653842, 0.0462196808149285, 0.102652055408985, -0.0513260312811916, -0.122568257224132, -0.205304109308448, -0.49027303035825, -1.23155391410031, 0.102652055404447, 0.245136514626631};

    Number ad_delta[3],
           ad_alpha[3],
           ad_omega[2],
            ad_zeta[7],
             ad_Psi[49];

    joint_bases::orth_poly g{2, false};

    joint_bases::bases_vector bases_rng;
    // raw poly of degree x with an intercept
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(2, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));

    // we get the correct value
    std::vector<std::vector<int> > ders{{0}, {0}, {0}};
    survival::expected_cum_hazzard comp_obj(g, bases_rng, 3, ders, false);
    std::vector<double> expansions(comp_obj.cache_mem_per_node() * n_nodes);
    {
      auto req_mem = comp_obj.n_wmem();
      double res = comp_obj(
        {ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
        wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
        nullptr);

      expect_true(pass_rel_err(res, true_val1, 1e-6));

      // with cached expansions
      comp_obj.cache_expansions
        (lb1, ub1, expansions.data(), wmem::get_double_mem(req_mem[1]),
         {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         expansions.data());
      expect_true(pass_rel_err(res, true_val1, 1e-6));
    }
    {
      auto req_mem = comp_obj.n_wmem();
      double res = comp_obj(
        {ns, ws, n_nodes}, lb2, ub2, z, delta, omega, alpha, zeta, Psi,
        wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
        nullptr);

      expect_true(pass_rel_err(res, true_val2, 1e-6));

      // with cached expansions
      comp_obj.cache_expansions
        (lb2, ub2, expansions.data(), wmem::get_double_mem(req_mem[1]),
        {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         expansions.data());
      expect_true(pass_rel_err(res, true_val2, 1e-6));
    }

    auto rewind_n_convert = [&]{
      Number::tape->rewind();
      cfaad::convertCollection(begin(delta), end(delta), ad_delta);
      cfaad::convertCollection(begin(omega), end(omega), ad_omega);
      cfaad::convertCollection(begin(alpha), end(alpha), ad_alpha);
      cfaad::convertCollection(begin(zeta), end(zeta), ad_zeta);
      cfaad::convertCollection(begin(Psi), end(Psi), ad_Psi);
    };

    // we get the correct gradient
    {
      rewind_n_convert();
      auto req_mem = comp_obj.n_wmem();
      Number res = comp_obj(
        {ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
        ad_Psi, wmem::get_Number_mem(req_mem[0]),
        wmem::get_double_mem(req_mem[1]), nullptr);

      expect_true(pass_rel_err(res.value(), true_val1, 1e-6));
      res.propagateToStart();
      double const *g{gr1};

      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));

      // with cached memory
      rewind_n_convert();
      comp_obj.cache_expansions
        (lb1, ub1, expansions.data(), wmem::get_double_mem(req_mem[1]),
         {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
         ad_Psi, wmem::get_Number_mem(req_mem[0]),
         wmem::get_double_mem(req_mem[1]), expansions.data());

      expect_true(pass_rel_err(res.value(), true_val1, 1e-6));
      res.propagateToStart();

      g = gr1;
      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
    }
    {
      rewind_n_convert();

      auto req_mem = comp_obj.n_wmem();
      Number res = comp_obj(
        {ns, ws, n_nodes}, lb2, ub2, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
        ad_Psi, wmem::get_Number_mem(req_mem[0]),
        wmem::get_double_mem(req_mem[1]), nullptr);

      expect_true(pass_rel_err(res.value(), true_val2, 1e-6));
      res.propagateToStart();
      double const *g{gr2};

      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));

      // with cached memory
      rewind_n_convert();
      comp_obj.cache_expansions
        (lb2, ub2, expansions.data(), wmem::get_double_mem(req_mem[1]),
        {ns, ws, n_nodes});
      res = comp_obj
        ({ns, ws, n_nodes}, lb2, ub2, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
         ad_Psi, wmem::get_Number_mem(req_mem[0]),
         wmem::get_double_mem(req_mem[1]), expansions.data());

      expect_true(pass_rel_err(res.value(), true_val2, 1e-6));
      res.propagateToStart();

      g = gr2;
      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
    }

    // clean-up
    wmem::clear_all();
  }

  test_that("expected_cum_hazzard gives the correct result with derivatives"){
    /*
     raw_poly <- function(x, degree, intercept){
     if(intercept)
     drop(outer(x, 0:degree, `^`))
     else
     drop(outer(x, 1:degree, `^`))
     }

     # parameters
     Z <- c(1, -.5, .33)
     delta <- c(.1, .2, -.3)
     g <- function(x) raw_poly(x, 2, FALSE)
     omega <- c(.2, -.33)
     alpha <- c(.1, .4, -.2)
     ms <- list(function(x) raw_poly(x, 1, TRUE),
     # account for integral
     function(x) raw_poly(x, 2, TRUE) * x / rep(1:3, each = length(x)),
     # account for derivative
     function(x) cbind(0, rep(1, length(x))))
     zeta <- c(-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7)
     set.seed(1)
     dput(Psi <- drop(round(rWishart(1, 16, diag(.025, 8)), 3)))
     stopifnot(all(eigen(Psi)$value > 0))

     f <- function(args, lb, ub){
     get_next <- function(n){
     out <- head(args, n)
     args <<- tail(args, -n)
     out
     }
     delta <- get_next(length(delta))
     omega <- get_next(length(omega))
     alpha <- get_next(length(alpha))
     zeta <- get_next(length(zeta))
     Psi <- matrix(args, NROW(Psi))

     integrand <- function(x){
     M <- matrix(0, length(zeta), length(ms) + 1)
     offset <- 0L
     for(i in seq_along(ms)){
     z <- ms[[i]](x)
     M[offset + seq_along(z), i] <- z
     offset <- offset + length(z)
     }
     M[length(zeta), length(ms) + 1L] <- 1

     M_alpha <- drop(M %*% c(alpha, 1))

     exp(delta %*% Z + g(x) %*% omega + M_alpha %*% zeta +
     M_alpha %*% Psi %*% M_alpha / 2)
     }

     integrate(Vectorize(integrand), lb, ub, rel.tol = 1e-10)$value
     }

     dput(f(c(delta, omega, alpha, zeta, Psi), 0, 2))
     dput(numDeriv::grad(f, c(delta, omega, alpha, zeta, Psi), lb = 0, ub = 2))

     dput(f(c(delta, omega, alpha, zeta, Psi), 1, 3))
     dput(numDeriv::grad(f, c(delta, omega, alpha, zeta, Psi), lb = 1, ub = 3))
     */

    constexpr double z[] {1, -.5, .33},
                 delta[] {.1, .2, -.3},
                 alpha[] {.1, .4, -.2},
                 omega[] {.2, -.33},
                  zeta[] {-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7 },
                   Psi[] {0.294, 0.109, -0.132, 0.049, -0.053, 0.037, -0.005, -0.009, 0.109, 0.588, -0.158, -0.017, -0.279, -0.131, 0.057, 0.042, -0.132, -0.158, 0.461, 0.132, 0.185, -0.01, 0.096, -0.01, 0.049, -0.017, 0.132, 0.333, 0.047, 0.038, -0.02, -0.119, -0.053, -0.279, 0.185, 0.047, 0.487, 0.067, -0.111, -0.057, 0.037, -0.131, -0.01, 0.038, 0.067, 0.296, -0.029, -0.058, -0.005, 0.057, 0.096, -0.02, -0.111, -0.029, 0.408, 0.035, -0.009, 0.042, -0.01, -0.119, -0.057, -0.058, 0.035, 0.237},
                   lb1   {0},
                   ub1   {2},
                   lb2   {1},
                   ub2   {3},
             true_val1   {3.51152119882823},
             true_val2   {5.19502423960864},
                   gr1[] {3.51152119909707, -1.75576059954854, 1.15880199563353, 3.16389312234803, 3.96791868818556, -1.23708360355054, 1.59084054922998, -0.569000165559629, 0.351152119531459, 0.316389312330735, 1.26555724779103, 0.793583737399411, 0.764070792425652, 0, -0.70230424011111, 3.51152119879396, 0.0175576059817869, 0.0158194661258817, 0.0632778623483541, 0.0396791872681199, 0.0382035388132134, 0, -0.0351152086365295, 0.17557606176869, 0.0158194661258817, 0.0198395934003802, 0.0793583740386074, 0.057305307746283, 0.0596371978730387, 0, -0.0316389305638529, 0.158194655904305, 0.0632778623483541, 0.0793583740386074, 0.317433495050123, 0.229221237812724, 0.23854879054946, 0, -0.126555724788653, 0.632778625916063, 0.0396791872681199, 0.057305307746283, 0.229221237812724, 0.178911592941831, 0.195493834813792, 0, -0.0793583781481621, 0.396791868328111, 0.0382035388132134, 0.0596371978730387, 0.238548790549455, 0.195493834813792, 0.220812925149533, 0, -0.07640707899187, 0.382035396523836, 0, 0, 0, 0, 0, 0, 0, 0, -0.0351152086365295, -0.0316389305638529, -0.126555724788653, -0.0793583783586933, -0.07640707899187, 0, 0.070230423887445, -0.351152120203304, 0.17557606176869, 0.158194657126436, 0.632778625916063, 0.396791868328111, 0.382035396523836, 0, -0.351152120323607, 1.75576059948604},
                   gr2[] {5.19502423917055, -2.59751211958528, 1.71435799911393, 12.2574196067228, 30.9445805106753, -11.5997035639195, 61.0661870597654, -1.68972307127706, 0.519502424009714, 1.22574196069078, 4.90296784304663, 6.18891610312129, 10.8760203405542, 0, -1.03900484807608, 5.19502423967183, 0.0259751212209169, 0.0612870985202201, 0.245148392416322, 0.309445805387659, 0.543801017447262, 0, -0.0519502346771197, 0.259751207457675, 0.0612870985202201, 0.154722902446751, 0.618891610309277, 0.815701524161611, 1.47310479949976, 0, -0.122574196839885, 0.612870981647331, 0.245148392416322, 0.618891610309277, 2.47556644076306, 3.26280610234088, 5.89241919833558, 0, -0.490296784283489, 2.45148391305149, 0.309445804340118, 0.815701521142228, 3.26280610234087, 4.4193143987196, 8.12315697256269, 0, -0.618891612526633, 3.0944580509255, 0.543801018420738, 1.47310479971392, 5.89241919829006, 8.12315697291544, 15.110485269309, 0, -1.08760203444549, 5.43801017080392, 0, 0, 0, 0, 0, 0, 0, 0, -0.0519502432587716, -0.122574196844501, -0.490296783839269, -0.618891611697667, -1.08760203398308, 0, 0.103900484816461, -0.519502422103854, 0.259751207370128, 0.612870981841571, 2.45148391734247, 3.09445805048973, 5.43801017066539, 0, -0.519502423570412, 2.59751212020358};

    Number ad_delta[3],
           ad_alpha[3],
           ad_omega[2],
            ad_zeta[8],
            ad_Psi[64];

    joint_bases::orth_poly g{2, false};

    joint_bases::bases_vector bases_rng;
    // raw poly of degree x with an intercept
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(2, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));

    // we get the correct value
    std::vector<std::vector<int> > ders{{0}, {-1}, {1}};
    survival::expected_cum_hazzard comp_obj(g, bases_rng, 3, ders, true);
    std::vector<double> expansions(comp_obj.cache_mem_per_node() * n_nodes);
    {
      auto req_mem = comp_obj.n_wmem();
      double res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         nullptr);

      expect_true(pass_rel_err(res, true_val1, 1e-6));

      // with cached memory
      comp_obj.cache_expansions
        (lb1, ub1, expansions.data(), wmem::get_double_mem(req_mem[1]),
         {ns, ws, n_nodes});

      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         expansions.data());
      expect_true(pass_rel_err(res, true_val1, 1e-6));
    }
    {
      auto req_mem = comp_obj.n_wmem();
      double res = comp_obj(
      {ns, ws, n_nodes}, lb2, ub2, z, delta, omega, alpha, zeta, Psi,
      wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
      nullptr);

      expect_true(pass_rel_err(res, true_val2, 1e-6));

      // with cached memory
      comp_obj.cache_expansions
        (lb2, ub2, expansions.data(), wmem::get_double_mem(req_mem[1]),
        {ns, ws, n_nodes});

      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, delta, omega, alpha, zeta, Psi,
         wmem::get_double_mem(req_mem[0]), wmem::get_double_mem(req_mem[1]),
         expansions.data());
      expect_true(pass_rel_err(res, true_val2, 1e-6));
    }

    auto rewind_n_convert = [&]{
      Number::tape->rewind();
      cfaad::convertCollection(begin(delta), end(delta), ad_delta);
      cfaad::convertCollection(begin(omega), end(omega), ad_omega);
      cfaad::convertCollection(begin(alpha), end(alpha), ad_alpha);
      cfaad::convertCollection(begin(zeta), end(zeta), ad_zeta);
      cfaad::convertCollection(begin(Psi), end(Psi), ad_Psi);
    };

    // we get the correct gradient
    {
      rewind_n_convert();

      auto req_mem = comp_obj.n_wmem();
      Number res = comp_obj(
        {ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
        ad_Psi, wmem::get_Number_mem(req_mem[0]),
        wmem::get_double_mem(req_mem[1]), nullptr);

      expect_true(pass_rel_err(res.value(), true_val1, 1e-6));
      res.propagateToStart();
      double const *g{gr1};

      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));

      // we cached memory
      rewind_n_convert();
      comp_obj.cache_expansions
        (lb1, ub1, expansions.data(), wmem::get_double_mem(req_mem[1]),
         {ns, ws, n_nodes});

      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
         ad_Psi, wmem::get_Number_mem(req_mem[0]),
         wmem::get_double_mem(req_mem[1]), expansions.data());

      expect_true(pass_rel_err(res.value(), true_val1, 1e-6));
      res.propagateToStart();

      g = gr1;
      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
    }
    {
      rewind_n_convert();

      auto req_mem = comp_obj.n_wmem();
      Number res = comp_obj(
      {ns, ws, n_nodes}, lb2, ub2, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
      ad_Psi, wmem::get_Number_mem(req_mem[0]),
      wmem::get_double_mem(req_mem[1]), nullptr);

      expect_true(pass_rel_err(res.value(), true_val2, 1e-6));
      res.propagateToStart();
      double const *g{gr2};

      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));

      // with cached memory
      rewind_n_convert();
      comp_obj.cache_expansions
        (lb2, ub2, expansions.data(), wmem::get_double_mem(req_mem[1]),
        {ns, ws, n_nodes});

      res = comp_obj
        ({ns, ws, n_nodes}, lb1, ub1, z, ad_delta, ad_omega, ad_alpha, ad_zeta,
         ad_Psi, wmem::get_Number_mem(req_mem[0]),
         wmem::get_double_mem(req_mem[1]), expansions.data());

      expect_true(pass_rel_err(res.value(), true_val2, 1e-6));
      res.propagateToStart();

      g = gr2;
      for(auto &x : ad_delta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_omega)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_alpha)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_zeta)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
      for(auto &x : ad_Psi)
        expect_true(pass_rel_err(x.adjoint(), *g++, 1e-6));
    }

    // clean-up
    wmem::clear_all();
  }
}

context("survival_dat is correct") {
  test_that("survival_dat gives the correct result"){
    /*
     raw_poly <- function(x, degree, intercept){
     if(intercept)
     drop(outer(x, 0:degree, `^`))
     else
     drop(outer(x, 1:degree, `^`))
     }

     # parameters
     Zs <- list(
     matrix(c(1, -.5, .33, .4), 2),
     matrix(c(1, -1, .33), 1))
     delta1 <- c(.1, .33)
     delta2 <- .55
     gs <- list(function(x) raw_poly(x, 2, FALSE),
     function(x) raw_poly(x, 1, FALSE))
     omega1 <- c(.2, -.33)
     omega2 <- c(.43)
     alpha1 <- c(.1, .4, -.2)
     alpha2 <- c(.11, -.2, .25)
     ms <- list(function(x) raw_poly(x, 1, TRUE),
     function(x) raw_poly(x, 2, TRUE),
     function(x) raw_poly(x, 1, TRUE))
     zeta <- c(-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7, .22)
     set.seed(1)
     dput(Psi <- drop(round(rWishart(1, 16, diag(.025, 9)), 3)))
     stopifnot(all(eigen(Psi)$value > 0))

     obs_info <- list(
     list(type = 1, y = 1, lower = 0, upper = 1.33, idx = 1L),
     list(type = 2, y = 0, lower = 0, upper = 2.1, idx = 1L),
     list(type = 1, y = 0, lower = 1, upper = 2.5, idx = 2L),
     list(type = 2, y = 1, lower = .67, upper = 1.8, idx = 2L),
     list(type = 2, y = 1, lower = .25, upper = 1.1, idx = 3L))

     f <- function(args){
     get_next <- function(n){
     out <- head(args, n)
     args <<- tail(args, -n)
     out
     }
     delta1 <- get_next(length(delta1))
     omega1 <- get_next(length(omega1))
     alpha1 <- get_next(length(alpha1))

     delta2 <- get_next(length(delta2))
     omega2 <- get_next(length(omega2))
     alpha2 <- get_next(length(alpha2))

     ds <- list(delta1, delta2)
     os <- list(omega1, omega2)
     as <- list(alpha1, alpha2)

     zeta <- get_next(length(zeta))
     Psi <- matrix(args, NROW(Psi))

     out <- 0
     for(i in seq_along(obs_info)){
     info <- obs_info[[i]]
     type <- info$type
     idx <- info$idx
     lower <- info$lower
     upper <- info$upper
     rng_remove <- ifelse(type == 1L, -length(zeta), -length(zeta) + 1L)
     zeta_use <- zeta[rng_remove]
     Psi_use <- Psi[rng_remove, rng_remove]

     if(info$y == 1){
     M <- matrix(0, length(zeta_use) - 1L, length(ms))
     offset <- 0L
     for(j in seq_along(ms)){
     z <- ms[[j]](upper)
     M[offset + seq_along(z), j] <- z
     offset <- offset + length(z)
     }
# compute the approximate expected log hazard
     out <- out - Zs[[type]][, idx] %*% ds[[type]] -
     gs[[type]](upper) %*% os[[type]] -
     as[[type]] %*% crossprod(M, zeta[seq_len(NROW(M))]) -
     tail(zeta_use, 1)
     }

     integrand <- function(x){
     M <- matrix(0, length(zeta_use), length(ms) + 1)
     offset <- 0L
     for(i in seq_along(ms)){
     z <- ms[[i]](x)
     M[offset + seq_along(z), i] <- z
     offset <- offset + length(z)
     }
     M[length(zeta_use), length(ms) + 1L] <- 1

     M_alpha <- drop(M %*% c(as[[type]], 1))

     exp(ds[[type]] %*% Zs[[type]][, idx] +
     gs[[type]](x) %*% os[[type]] +
     M_alpha %*% zeta_use +
     M_alpha %*% Psi_use %*% M_alpha / 2)
     }

     out <- out + integrate(Vectorize(integrand), lower, upper,
     rel.tol = 1e-10)$value
     }

     out
     }

     dput(f(c(delta1, omega1, alpha1, delta2, omega2, alpha2, zeta, Psi)))
     dput(numDeriv::grad(
     f, c(delta1, omega1, alpha1, delta2, omega2, alpha2, zeta, Psi)))
     */
    constexpr unsigned n_obs[] {2, 3},
                     n_fixef[] {2, 1};

    constexpr double delta1[] {.1, .33},
                     delta2[] {.55},
                     omega1[] {.2, -.33},
                     omega2[] {.43},
                     alpha1[] {.1, .4, -.2},
                     alpha2[] {.11, -.2, .25},
                       zeta[] {-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7, .22},
                        Psi[] {0.294, 0.109, -0.132, 0.049, -0.053, 0.037, -0.005, -0.009, 0.065, 0.109, 0.588, -0.158, -0.017, -0.279, -0.131, 0.057, 0.042, 0.005, -0.132, -0.158, 0.461, 0.132, 0.185, -0.01, 0.096, -0.01, -0.05, 0.049, -0.017, 0.132, 0.333, 0.047, 0.038, -0.02, -0.119, 0.059, -0.053, -0.279, 0.185, 0.047, 0.487, 0.067, -0.111, -0.057, 0.039, 0.037, -0.131, -0.01, 0.038, 0.067, 0.296, -0.029, -0.058, -0.031, -0.005, 0.057, 0.096, -0.02, -0.111, -0.029, 0.408, 0.035, -0.104, -0.009, 0.042, -0.01, -0.119, -0.057, -0.058, 0.035, 0.237, -0.001, 0.065, 0.005, -0.05, 0.059, 0.039, -0.031, -0.104, -0.001, 0.357},
                       lbs1[] {0, 1},
                       ubs1[] {1.33, 2.5},
                     event1[] {1, 0},
                       lbs2[] {0, .67, .25},
                       ubs2[] {2.1, 1.8, 1.1},
                     event2[] {0, 1, 1},
                     true_val {18.128321562534},
                  true_grad[] {
                    // the marker parameters
                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                    // the survival time outcomes
                    2.77911582884371, 0.47577831842005, 6.20195485243171, 10.6023870328148, -3.85281038038304, 12.0426817828313, -1.98852882249393, 9.33216853215579, 13.093206859266, 2.69935119697421, -14.3955407877077, 0.893176711420481,
                    // marker term error covariance matrix
                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                    // the shared random effect covariance matrix
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    // the frailty term covariance matrix
                    0, 0, 0, 0,
                    // the VA mean and covariance matrix
                    1.77981519694067, 2.06044824038661, -0.332110561721501, -0.137859435136169, 0.504511469459273, 1.91247044054486, 2.03291074553718, 4.99110746843772, 11.6427677338483, 0.112494282017397, 0.134418673405233, -0.0302482946791018, -0.0252861806670186, -0.00702864639851131, 0.127676977832993, 0.144587020423434, 0.299555375073688, 0.750352226133017, 0.134418673405233, 0.201806346559603, -0.0252861776705658, -0.00702863672707872, 0.0511614226467864, 0.144587047353163, 0.194355107982903, 0.37659774384541, 0.879626308365913, -0.0302482946791018, -0.0252861776705658, 0.752143952208822, 0.922420528005028, 1.452347297081, -0.58071353427628, -0.701108367225872, 1.19822151745652, -1.36427676853407, -0.0252861806670186, -0.00702863672707872, 0.922420528005028, 1.45234729727697, 2.57490502487397, -0.70110837336599, -1.07315691217169, 1.50639097057185, -1.5993206849611, -0.00702864639851131, 0.0511614219108794, 1.45234729707531, 2.57490502487397, 4.91066601063021, -1.0731568989341, -1.84298204250653, 2.47425740323255, -2.31322166810494, 0.12767698802086, 0.144587047610301, -0.580713537644153, -0.70110837336599, -1.073156898934, 0.546158640498308, 0.650426813035474, -0.59911075014674, 1.70534596961225, 0.144587027160432, 0.194355107983013, -0.701108367225872, -1.07315691217169, -1.84298204250653, 0.650426813035474, 0.9703075134438, -0.753195488343061, 1.99915085651155, 0.299555375073688, 0.37659774384541, 1.19822151745652, 1.50639097085492, 2.47425740321408, -0.59911075014674, -0.753195488343061, 2.99555373327866, 0, 0.750352226133017, 0.879626308576444, -1.36427676853407, -1.5993206849611, -2.31322166810494, 1.70534596961225, 1.99915085651155, 0, 6.82138386757711};

    double Z1[] {1, -.5, .33, .4},
           Z2[] {1, -1, .33};

    joint_bases::bases_vector bases_fix;
    // raw poly of degree x without an intercept
    bases_fix.emplace_back(new joint_bases::orth_poly{2, false});
    bases_fix.emplace_back(new joint_bases::orth_poly{1, false});

    joint_bases::bases_vector bases_rng;
    // raw poly of degree x with an intercept
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(2, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));

    std::vector<survival::obs_input> surv_input;
    surv_input.emplace_back
      (survival::obs_input{n_obs[0], lbs1, ubs1, event1});
    surv_input.emplace_back
      (survival::obs_input{n_obs[1], lbs2, ubs2, event2});

    subset_params par_idx;
    par_idx.add_marker({1, 1, bases_rng[0]->n_basis()});
    par_idx.add_marker({2, 2, bases_rng[1]->n_basis()});
    par_idx.add_marker({2, 1, bases_rng[2]->n_basis()});

    for(unsigned i = 0; i < 2; ++i)
      par_idx.add_surv({n_fixef[i], bases_fix[i]->n_basis(), {1, 1, 1}, true});

    std::vector<simple_mat<double> > design_mats;
    design_mats.emplace_back(Z1, n_fixef[0], n_obs[0]);
    design_mats.emplace_back(Z2, n_fixef[1], n_obs[1]);

    std::vector<std::vector<std::vector<int> > > ders
      {{{0}, {0}, {0}}, {{0}, {0}, {0}}};
    survival::survival_dat comp_obj(bases_fix, bases_rng, design_mats,
                                    par_idx, surv_input, ders);

    // basic checks
    expect_true(comp_obj.n_terms(0) == n_obs[0]);
    expect_true(comp_obj.n_terms(1) == n_obs[1]);
    expect_true(comp_obj.n_outcomes() == 2);

    // compute the lower bound
    std::vector<double> par(par_idx.n_params_w_va(), 0);
    std::copy(begin(delta1), end(delta1), begin(par) + par_idx.fixef_surv(0));
    std::copy(begin(delta2), end(delta2), begin(par) + par_idx.fixef_surv(1));

    std::copy(begin(omega1), end(omega1),
              begin(par) + par_idx.fixef_vary_surv(0));
    std::copy(begin(omega2), end(omega2),
              begin(par) + par_idx.fixef_vary_surv(1));

    std::copy(begin(alpha1), end(alpha1), begin(par) + par_idx.association(0));
    std::copy(begin(alpha2), end(alpha2), begin(par) + par_idx.association(1));

    std::copy(begin(zeta), end(zeta), par.begin() + par_idx.va_mean());
    std::copy(begin(Psi), end(Psi), par.begin() + par_idx.va_vcov());

    // we get the right value
    {
      auto req_wmem = comp_obj.n_wmem();
      double res{};
      for(vajoint_uint i = 0; i < 2; ++i)
        for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
          res += comp_obj
            (par.data(), wmem::get_double_mem(req_wmem[0]), j, i,
             wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

      expect_true(pass_rel_err(res, true_val, 1e-6));
    }
    {
      // with caching of the quadrature rule
      comp_obj.set_cached_expansions({ns, ws, n_nodes});
      auto req_wmem = comp_obj.n_wmem();
      double res{};
      for(vajoint_uint i = 0; i < 2; ++i)
        for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
          res += comp_obj
          (par.data(), wmem::get_double_mem(req_wmem[0]), j, i,
           wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

      expect_true(pass_rel_err(res, true_val, 1e-6));

      comp_obj.clear_cached_expansions();
    }

    // we get the right gradient
    {
      Number::tape->rewind();
      std::vector<Number> ad_par(par.size());
      cfaad::convertCollection(par.begin(), par.end(), ad_par.begin());

      auto req_wmem = comp_obj.n_wmem();
      Number res{0};
      for(vajoint_uint i = 0; i < 2; ++i)
        for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
          res += comp_obj
            (ad_par.data(), wmem::get_Number_mem(req_wmem[0]), j, i,
             wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

      expect_true(pass_rel_err(res.value(), true_val, 1e-6));
      expect_true(
        ad_par.size() == static_cast<size_t>(
          std::distance(begin(true_grad), end(true_grad))));

      res.propagateToStart();
      for(size_t i = 0; i < ad_par.size(); ++i)
        expect_true(pass_rel_err(ad_par[i].adjoint(), true_grad[i], 1e-6));
    }

    // works with caching
    comp_obj.set_cached_expansions({ns, ws, n_nodes});

    Number::tape->rewind();
    std::vector<Number> ad_par(par.size());
    cfaad::convertCollection(par.begin(), par.end(), ad_par.begin());

    auto req_wmem = comp_obj.n_wmem();
    Number res{0};
    for(vajoint_uint i = 0; i < 2; ++i)
      for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
        res += comp_obj
        (ad_par.data(), wmem::get_Number_mem(req_wmem[0]), j, i,
         wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

    expect_true(pass_rel_err(res.value(), true_val, 1e-6));
    expect_true(
      ad_par.size() == static_cast<size_t>(
        std::distance(begin(true_grad), end(true_grad))));

    res.propagateToStart();
    for(size_t i = 0; i < ad_par.size(); ++i)
      expect_true(pass_rel_err(ad_par[i].adjoint(), true_grad[i], 1e-6));

    // clean up
    wmem::clear_all();
  }

  test_that("survival_dat gives the correct result without one frailty"){
    /*
     raw_poly <- function(x, degree, intercept){
     if(intercept)
     drop(outer(x, 0:degree, `^`))
     else
     drop(outer(x, 1:degree, `^`))
     }

# parameters
     Zs <- list(
     matrix(c(1, -.5, .33, .4), 2),
     matrix(c(1, -1, .33), 1))
     delta1 <- c(.1, .33)
     delta2 <- .55
     gs <- list(function(x) raw_poly(x, 2, FALSE),
     function(x) raw_poly(x, 1, FALSE))
     omega1 <- c(.2, -.33)
     omega2 <- c(.43)
     alpha1 <- c(.1, .4, -.2)
     alpha2 <- c(.11, -.2, .25)
     ms <- list(function(x) raw_poly(x, 1, TRUE),
     function(x) raw_poly(x, 2, TRUE),
     function(x) raw_poly(x, 1, TRUE))
     zeta <- c(-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7)
     set.seed(1)
     dput(Psi <- drop(round(rWishart(1, 16, diag(.025, 8)), 3)))
     stopifnot(all(eigen(Psi)$value > 0))

     obs_info <- list(
     list(type = 1, y = 1, lower = 0, upper = 1.33, idx = 1L),
     list(type = 2, y = 0, lower = 0, upper = 2.1, idx = 1L),
     list(type = 1, y = 0, lower = 1, upper = 2.5, idx = 2L),
     list(type = 2, y = 1, lower = .67, upper = 1.8, idx = 2L),
     list(type = 2, y = 1, lower = .25, upper = 1.1, idx = 3L))

     f <- function(args){
     get_next <- function(n){
     out <- head(args, n)
     args <<- tail(args, -n)
     out
     }
     delta1 <- get_next(length(delta1))
     omega1 <- get_next(length(omega1))
     alpha1 <- get_next(length(alpha1))

     delta2 <- get_next(length(delta2))
     omega2 <- get_next(length(omega2))
     alpha2 <- get_next(length(alpha2))

     ds <- list(delta1, delta2)
     os <- list(omega1, omega2)
     as <- list(alpha1, alpha2)

     zeta <- get_next(length(zeta))
     Psi <- matrix(args, NROW(Psi))

     out <- 0
     for(i in seq_along(obs_info)){
     info <- obs_info[[i]]
     type <- info$type
     idx <- info$idx
     lower <- info$lower
     upper <- info$upper
     with_frailty <- type == 2L
     rng_remove <- ifelse(with_frailty, TRUE, -length(zeta))
     zeta_use <- zeta[rng_remove]
     Psi_use <- Psi[rng_remove, rng_remove]

     if(info$y == 1){
     M <- matrix(0, length(zeta_use) - with_frailty, length(ms))
     offset <- 0L
     for(j in seq_along(ms)){
     z <- ms[[j]](upper)
     M[offset + seq_along(z), j] <- z
     offset <- offset + length(z)
     }
# compute the approximate expected log hazard
     out <- out - Zs[[type]][, idx] %*% ds[[type]] -
     gs[[type]](upper) %*% os[[type]] -
     as[[type]] %*% crossprod(M, zeta[seq_len(NROW(M))])

     if(with_frailty)
     out <- out - tail(zeta_use, 1)
     }

     integrand <- function(x){
     M <- matrix(0, length(zeta_use), length(ms) + with_frailty)
     offset <- 0L
     for(i in seq_along(ms)){
     z <- ms[[i]](x)
     M[offset + seq_along(z), i] <- z
     offset <- offset + length(z)
     }
     if(with_frailty)
     M[length(zeta_use), length(ms) + 1L] <- 1

     M_alpha <-
     drop(if(with_frailty) M %*% c(as[[type]], 1) else  M %*% as[[type]])

     exp(ds[[type]] %*% Zs[[type]][, idx] +
     gs[[type]](x) %*% os[[type]] +
     M_alpha %*% zeta_use +
     M_alpha %*% Psi_use %*% M_alpha / 2)
     }

     out <- out + integrate(Vectorize(integrand), lower, upper,
     rel.tol = 1e-10)$value
     }

     out
     }

     dput(f(c(delta1, omega1, alpha1, delta2, omega2, alpha2, zeta, Psi)))
     dput(numDeriv::grad(
     f, c(delta1, omega1, alpha1, delta2, omega2, alpha2, zeta, Psi)))
     */
    constexpr unsigned n_obs[] {2, 3},
                     n_fixef[] {2, 1};

    constexpr double delta1[] {.1, .33},
                     delta2[] {.55},
                     omega1[] {.2, -.33},
                     omega2[] {.43},
                     alpha1[] {.1, .4, -.2},
                     alpha2[] {.11, -.2, .25},
                       zeta[] {-0.1, -0.186, -0.049, 0.015, -0.056, 0.114, -0.126, 0.7},
                        Psi[] {0.294, 0.109, -0.132, 0.049, -0.053, 0.037, -0.005, -0.009, 0.109, 0.588, -0.158, -0.017, -0.279, -0.131, 0.057, 0.042, -0.132, -0.158, 0.461, 0.132, 0.185, -0.01, 0.096, -0.01, 0.049, -0.017, 0.132, 0.333, 0.047, 0.038, -0.02, -0.119, -0.053, -0.279, 0.185, 0.047, 0.487, 0.067, -0.111, -0.057, 0.037, -0.131, -0.01, 0.038, 0.067, 0.296, -0.029, -0.058, -0.005, 0.057, 0.096, -0.02, -0.111, -0.029, 0.408, 0.035, -0.009, 0.042, -0.01, -0.119, -0.057, -0.058, 0.035, 0.237},
                       lbs1[] {0, 1},
                       ubs1[] {1.33, 2.5},
                     event1[] {1, 0},
                       lbs2[] {0, .67, .25},
                       ubs2[] {2.1, 1.8, 1.1},
                     event2[] {0, 1, 1},
                     true_val {24.1027955389656},
                  true_grad[] {
                    // the marker parameters
                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                    // the survival time outcomes
                    0.796489628374787, 0.566166623999451, 2.5223152317401, 4.71799191288861, -1.97186637683011, 7.35876970167952, -0.986179849357635, 15.3055929373714, 24.8464912248746, 3.62972546408691, -33.6971706049407, 4.68619752253532,
                    // marker term error covariance matrix
                    0, 0, 0, 0, 0, 0, 0, 0, 0,
                    // the shared random effect covariance matrix
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    // the frailty term covariance matrix
                    0,
                    // the VA mean and covariance matrix
                    2.49611713056971, 2.98534555593684, -3.40959306044233, -3.96037211827117, -5.40185647634367, 4.84402970668841, 5.70715975546779, 20.9282212086845, 0.153416379414407, 0.187127849697932, -0.19340787031736, -0.228165098909253, -0.320110096012648, 0.285861755211431, 0.342991107256495, 1.26105219974235, 0.18712784970759, 0.279850820419408, -0.228165099935113, -0.320110069448553, -0.484578164801837, 0.342991102230666, 0.497440989369615, 1.52605701051957, -0.19340787031736, -0.228165099935113, 0.693774661705644, 0.863115045206594, 1.33685667913844, -0.690810651350491, -0.847754894619718, -2.2928221136862, -0.228165098909253, -0.320110069448553, 0.863115045206594, 1.33685667778016, 2.30410257868686, -0.847754893473597, -1.28185731766474, -2.77464912109116, -0.320110096012648, -0.484578165537744, 1.33685667913844, 2.30410257797016, 4.2438822063881, -1.28185733104135, -2.14636736720533, -4.08952662559268, 0.285861755211431, 0.342991102230666, -0.690810651350491, -0.847754893473597, -1.28185733105706, 0.775309472472442, 0.944124159596086, 2.86602764648663, 0.342991113993493, 0.497440992953218, -0.847754892480988, -1.28185731766505, -2.14636736720539, 0.944124159596086, 1.40771490716505, 3.46831141291736, 1.26105217692924, 1.52605701051957, -2.29282211715996, -2.77464911908273,  -4.08952661838854, 2.86602764648663, 3.46831141291736, 11.4641106041719};

    double Z1[] {1, -.5, .33, .4},
           Z2[] {1, -1, .33};

    joint_bases::bases_vector bases_fix;
    // raw poly of degree x without an intercept
    bases_fix.emplace_back(new joint_bases::orth_poly{2, false});
    bases_fix.emplace_back(new joint_bases::orth_poly{1, false});

    joint_bases::bases_vector bases_rng;
    // raw poly of degree x with an intercept
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(2, true));
    bases_rng.emplace_back(new joint_bases::orth_poly(1, true));

    std::vector<survival::obs_input> surv_input;
    surv_input.emplace_back
      (survival::obs_input{n_obs[0], lbs1, ubs1, event1});
    surv_input.emplace_back
      (survival::obs_input{n_obs[1], lbs2, ubs2, event2});

    subset_params par_idx;
    par_idx.add_marker({1, 1, bases_rng[0]->n_basis()});
    par_idx.add_marker({2, 2, bases_rng[1]->n_basis()});
    par_idx.add_marker({2, 1, bases_rng[2]->n_basis()});

    par_idx.add_surv({n_fixef[0], bases_fix[0]->n_basis(), {1, 1, 1}, false});
    par_idx.add_surv({n_fixef[1], bases_fix[1]->n_basis(), {1, 1, 1}, true});

    std::vector<simple_mat<double> > design_mats;
    design_mats.emplace_back(Z1, n_fixef[0], n_obs[0]);
    design_mats.emplace_back(Z2, n_fixef[1], n_obs[1]);

    std::vector<std::vector<std::vector<int> > > ders
      {{{0}, {0}, {0}}, {{0}, {0}, {0}}};
    survival::survival_dat comp_obj(bases_fix, bases_rng, design_mats,
                                    par_idx, surv_input, ders);

    // basic checks
    expect_true(comp_obj.n_terms(0) == n_obs[0]);
    expect_true(comp_obj.n_terms(1) == n_obs[1]);
    expect_true(comp_obj.n_outcomes() == 2);

    // compute the lower bound
    std::vector<double> par(par_idx.n_params_w_va(), 0);
    std::copy(begin(delta1), end(delta1), begin(par) + par_idx.fixef_surv(0));
    std::copy(begin(delta2), end(delta2), begin(par) + par_idx.fixef_surv(1));

    std::copy(begin(omega1), end(omega1),
              begin(par) + par_idx.fixef_vary_surv(0));
    std::copy(begin(omega2), end(omega2),
              begin(par) + par_idx.fixef_vary_surv(1));

    std::copy(begin(alpha1), end(alpha1), begin(par) + par_idx.association(0));
    std::copy(begin(alpha2), end(alpha2), begin(par) + par_idx.association(1));

    std::copy(begin(zeta), end(zeta), par.begin() + par_idx.va_mean());
    std::copy(begin(Psi), end(Psi), par.begin() + par_idx.va_vcov());

    // we get the right value
    {
      auto req_wmem = comp_obj.n_wmem();
      double res{};
      for(vajoint_uint i = 0; i < 2; ++i)
        for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
          res += comp_obj
            (par.data(), wmem::get_double_mem(req_wmem[0]), j, i,
             wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

      expect_true(pass_rel_err(res, true_val, 1e-6));
    }
    {
      // with caching of the quadrature rule
      comp_obj.set_cached_expansions({ns, ws, n_nodes});
      auto req_wmem = comp_obj.n_wmem();
      double res{};
      for(vajoint_uint i = 0; i < 2; ++i)
        for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
          res += comp_obj
          (par.data(), wmem::get_double_mem(req_wmem[0]), j, i,
           wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

      expect_true(pass_rel_err(res, true_val, 1e-6));

      comp_obj.clear_cached_expansions();
    }

    // we get the right gradient
    {
      Number::tape->rewind();
      std::vector<Number> ad_par(par.size());
      cfaad::convertCollection(par.begin(), par.end(), ad_par.begin());

      auto req_wmem = comp_obj.n_wmem();
      Number res{0};
      for(vajoint_uint i = 0; i < 2; ++i)
        for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
          res += comp_obj
            (ad_par.data(), wmem::get_Number_mem(req_wmem[0]), j, i,
             wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

      expect_true(pass_rel_err(res.value(), true_val, 1e-6));
      expect_true(
        ad_par.size() == static_cast<size_t>(
          std::distance(begin(true_grad), end(true_grad))));

      res.propagateToStart();
      for(size_t i = 0; i < ad_par.size(); ++i)
        expect_true(pass_rel_err(ad_par[i].adjoint(), true_grad[i], 1e-6));
    }

    // works with caching
    comp_obj.set_cached_expansions({ns, ws, n_nodes});

    Number::tape->rewind();
    std::vector<Number> ad_par(par.size());
    cfaad::convertCollection(par.begin(), par.end(), ad_par.begin());

    auto req_wmem = comp_obj.n_wmem();
    Number res{0};
    for(vajoint_uint i = 0; i < 2; ++i)
      for(vajoint_uint j = 0; j < comp_obj.n_terms(i); ++j)
        res += comp_obj
        (ad_par.data(), wmem::get_Number_mem(req_wmem[0]), j, i,
         wmem::get_double_mem(req_wmem[1]), {ns, ws, n_nodes});

    expect_true(pass_rel_err(res.value(), true_val, 1e-6));
    expect_true(
      ad_par.size() == static_cast<size_t>(
        std::distance(begin(true_grad), end(true_grad))));

    res.propagateToStart();
    for(size_t i = 0; i < ad_par.size(); ++i)
      expect_true(pass_rel_err(ad_par[i].adjoint(), true_grad[i], 1e-6));

    // clean up
    wmem::clear_all();
  }
}
