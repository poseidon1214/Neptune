#ifndef _LBFGS_H_
#define _LBFGS_H_

namespace maxent {

//template<class FuncGrad>
//std::vector<double> 
//perform_LBFGS(FuncGrad func_grad, const std::vector<double> & x0);

std::vector<double> 
perform_LBFGS(double (*func_grad)(const std::vector<double> &, std::vector<double> &), 
	      const std::vector<double> & x0);


std::vector<double> 
perform_OWLQN(double (*func_grad)(const std::vector<double> &, std::vector<double> &), 
	      const std::vector<double> & x0,
	      const double C);

//const int    LBFGS_M = 7;
const int    LBFGS_M = 10;

}  // namespace maxent

#endif
