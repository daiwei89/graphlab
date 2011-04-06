#include <vector>
#include <algorithm>
#include <iostream>


#include <cxxtest/TestSuite.h>


#include <graphlab/parallel/pthread_tools.hpp>

#include <graphlab/factors/unary_factor.hpp>
#include <graphlab/factors/binary_factor.hpp>
#include <graphlab/factors/table_factor.hpp>


using namespace graphlab;


class factor_tests : public CxxTest::TestSuite {
public:

  void test_variables() {
    std::cout << "Test Variables" << std::endl;
    discrete_variable v1(1, 3);
    std::cout << v1 << std::endl;

    discrete_variable v2(2, 4);
    std::cout << v2 << std::endl;
  
    discrete_variable v3(3, 2);
    std::cout << v3 << std::endl;

    TS_ASSERT_EQUALS( v1, v1 );
    TS_ASSERT_DIFFERS( v1, v2 );
    TS_ASSERT_DIFFERS( v1, v3 );
    TS_ASSERT_LESS_THAN(v1, v2);
    TS_ASSERT_LESS_THAN(v2, v3);       
  }

  void test_domain() {
    std::cout << "Test domain" << std::endl;
    const size_t max_dim = 5;
    typedef discrete_domain<max_dim> domain_type;
    
    discrete_variable v1(1, 3);
    discrete_variable v2(2, 4);
    discrete_variable v3(3, 2);
    discrete_variable v4(4, 2);
    domain_type dom0;
    domain_type dom1(v1);
    domain_type dom2(v1,v3);
    domain_type dom3(v1,v2,v3);
    std::vector<discrete_variable> vec;
    vec.push_back(v3); vec.push_back(v1); vec.push_back(v2);
    domain_type dom4(vec);
    TS_ASSERT_EQUALS(dom3, dom4);
    domain_type dom5 = dom0 + dom2;
    TS_ASSERT_EQUALS(dom5, dom2);
    domain_type dom6 = dom2 + dom2;
    TS_ASSERT_EQUALS(dom6, dom2);
    domain_type dom7(v3);
    TS_ASSERT_EQUALS(dom2 - dom1, dom7);
    TS_ASSERT_EQUALS(dom2 - dom7, dom1);
    TS_ASSERT_EQUALS(dom3 - v2, dom2);
    TS_ASSERT_EQUALS(dom2 + dom1 + dom3, dom3);

    TS_ASSERT_EQUALS(dom2.intersect(dom3), dom2);
    domain_type dom9(v2, v3);
    TS_ASSERT_EQUALS(dom2.intersect(dom3), dom2);
    TS_ASSERT_EQUALS(dom2.intersect(dom7), dom7);
    TS_ASSERT_EQUALS(dom0.intersect(dom3), dom0);
    TS_ASSERT_EQUALS(dom2.intersect(dom9), dom7);
  }

  void test_assignment() {
    std::cout << "Test domain: " << std::endl;
    const size_t max_dim = 5;
    typedef discrete_domain<max_dim> domain_type;
    typedef discrete_assignment<max_dim> assignment_type;
    
    discrete_variable v1(1,11), v2(2,10), v3(3,8), v4(4,3), v5(5,2);
    domain_type dom(v1,v2,v3);
    domain_type sub_dom(v1,v3);
    size_t i = 0;
    size_t val[3];
    for(size_t j = 0; j < 3; ++j) val[j] = 0;
    for(assignment_type asg = dom.begin();
        asg < dom.end(); ++asg, ++i) {
      TS_ASSERT_EQUALS(asg.linear_index(), i);
      TS_ASSERT_EQUALS(asg.asg_at(0), asg.asg(1));
      TS_ASSERT_EQUALS(asg.asg_at(1), asg.asg(2));
      TS_ASSERT_EQUALS(asg.asg_at(2), asg.asg(3));
      TS_ASSERT_EQUALS(val[0], asg.asg(1));
      TS_ASSERT_EQUALS(val[1], asg.asg(2));
      TS_ASSERT_EQUALS(val[2], asg.asg(3));
      for(size_t j = 0; j < 3; ++j) {
        if(val[j] < dom.var(j).size() - 1) {
          val[j]++;
          break;
        } else val[j] = 0;
      }
      assignment_type rev_asg(dom, asg.linear_index());
      TS_ASSERT_EQUALS(rev_asg.asg(1), asg.asg(1));
      TS_ASSERT_EQUALS(rev_asg.asg(2), asg.asg(2));
      TS_ASSERT_EQUALS(rev_asg.asg(3), asg.asg(3));
      TS_ASSERT_EQUALS(rev_asg.linear_index(), asg.linear_index());
      assignment_type other_asg = asg.restrict(sub_dom);
      TS_ASSERT_EQUALS(other_asg.asg(1), asg.asg(1));
      TS_ASSERT_EQUALS(other_asg.asg(3), asg.asg(3));
      assignment_type asg3(dom);
      asg3.set_asg(1, asg.asg(1));
      asg3.set_asg(2, asg.asg(2));
      asg3.set_asg(3, asg.asg(3));
      TS_ASSERT_EQUALS(asg3.asg(1), asg.asg(1));
      TS_ASSERT_EQUALS(asg3.asg(2), asg.asg(2));
      TS_ASSERT_EQUALS(asg3.asg(3), asg.asg(3));
      domain_type dom2(v4, v5);
      for(assignment_type asg2 = dom2.begin();  asg2 < dom2.end(); ++asg2) {
        assignment_type joint = asg & asg2;
        TS_ASSERT_EQUALS(joint.asg(4), asg2.asg(4));
        TS_ASSERT_EQUALS(joint.asg(5), asg2.asg(5));
      }

      assignment_type asg4(sub_dom);
      asg4.update(asg);
      TS_ASSERT_EQUALS(asg4, asg.restrict(sub_dom));
      
    }

    
    
  }

  
  
  void test_table_factor() {
    std::cout << "Testing factors" << std::endl;
    const size_t max_dim = 5;
    typedef graphlab::table_factor<max_dim> factor_type;
    typedef factor_type::domain_type domain_type;
    typedef factor_type::assignment_type assignment_type;

    
    discrete_variable v1(1,3), v2(2,2), v3(3,2), v4(4,3), v5(5,2);
    domain_type dom(v1,v2,v3);

    // Create a factor over the domain
    factor_type factor(dom);
    factor.uniform();
    double sum = 0;
    double sum2 = 0;
    for(assignment_type asg = dom.begin(); asg < dom.end(); ++asg) {
      sum += std::exp(factor.logP(asg));
      sum2 += std::exp(factor.logP(asg.linear_index()));
    }
    TS_ASSERT_EQUALS(sum, sum2);
    TS_ASSERT_LESS_THAN(std::abs(sum - 1), 1E-10);
   
    for(assignment_type asg = dom.begin(); asg < dom.end(); ++asg) {
      double val = double(rand()) / RAND_MAX;
      factor.logP(asg) = val;
      TS_ASSERT_EQUALS(factor.logP(asg), 
                       factor.logP(asg.linear_index()));
    }

    factor /= factor;

    for(assignment_type asg = dom.begin(); asg < dom.end(); ++asg) {
      TS_ASSERT_EQUALS(factor.logP(asg), 0);
    }

    factor *= factor;

    for(assignment_type asg = dom.begin(); asg < dom.end(); ++asg) {
      TS_ASSERT_EQUALS(factor.logP(asg), 0);
    }

    for(assignment_type asg = dom.begin(); asg < dom.end(); ++asg) {
      double val = double(rand()) / RAND_MAX;
      factor.logP(asg) = val;
      TS_ASSERT_EQUALS(factor.logP(asg), 
                       factor.logP(asg.linear_index()));
    }

    factor_type factor2 = factor * factor;
    factor_type factor3 = factor2 / factor;

    for(assignment_type asg = dom.begin(); asg < dom.end(); ++asg) {
      TS_ASSERT_EQUALS(factor2.logP(asg)/2, factor.logP(asg));
      TS_ASSERT_EQUALS(factor3.logP(asg), factor.logP(asg));
      TS_ASSERT_EQUALS(factor2.logP(asg)/2, factor.logP(asg.linear_index()));
      TS_ASSERT_EQUALS(factor3.logP(asg), factor.logP(asg.linear_index()));
    }
    
    factor.normalize();


    factor_type factor4( domain_type(v1, v3) );
    factor4.marginalize(factor);
    

    factor_type factor5( domain_type(v1, v3) );
    domain_type d2(v2);
    factor5.zero();

    for(assignment_type asg = d2.begin(); asg < d2.end(); ++asg) {
      factor_type tmp(domain_type(v1,v3));
      tmp.condition(factor, asg);
      for(assignment_type asg2 = tmp.args().begin(); 
          asg2 < tmp.args().end(); ++asg2) {
        factor5.logP(asg2) += std::exp(tmp.logP(asg2));
      }
    } 

    for(size_t i = 0; i < factor5.size(); ++i) {
      factor5.logP(i) = std::log(factor5.logP(i));
      TS_ASSERT_EQUALS(factor5.logP(i), factor4.logP(i));
    }

  }


  // void test_table_factor_sample() {
  //   const size_t max_dim = 5;
  //   typedef graphlab::table_factor<max_dim> factor_type;
  //   typedef factor_type::domain_type domain_type;
  //   typedef factor_type::assignment_type assignment_type;
  //   discrete_variable v1(1, 3);
  //   discrete_variable v2(2, 4);
  //   discrete_variable v3(3, 2);

  //   domain_type dom(v1,v2,v3);
  //   // Create a factor over the domain
  //   factor_type factor(dom);
  //   factor.uniform();

  //   factor.logP(0) +=2;
  //   factor.logP(2) +=3;
  //   factor.logP(4) +=4;
  //   factor.logP(6) +=1;

  //   factor_type counts(factor.args());
  //   factor.normalize();
  //   size_t num_samples = 10000000;
  //   for(size_t i = 0; i < num_samples; ++i) {
  //     assignment_type asg = factor.sample();
  //     ++counts.logP(asg);
  //   }
  //   double sum = 0;
  //   for(size_t i = 0; i < counts.size(); ++i)
  //     sum += counts.logP(i);
  //   for(size_t i = 0; i < counts.size(); ++i) {
  //     counts.logP(i) = std::log( counts.logP(i) / sum );
  //   }
  //   std::cout << "True Factor: " << factor << std::endl;
  //   std::cout << "Sampled: " << counts << std::endl;
  //   for(size_t i = 0; i < counts.size(); ++i) {
  //     TS_ASSERT_LESS_THAN(std::abs(factor.logP(i) -
  //                                  counts.logP(i)) , 1E-2);
  //   }

  // }



  void test_unary_binary_factors() {

    unary_factor a(0, 5);
    unary_factor b(1, 7);
  
    binary_factor bin(0,5, 1,7);
    
    // initialize factors
    for(size_t i = 0; i < a.arity(); ++i) a.logP(i) = i;
    //  a.normalize();
    for(size_t i = 0; i < a.arity(); ++i)
      for(size_t j = 0; j < b.arity(); ++j)
        bin.logP(0, i, 1, j) = i + j  + 1;
  

    // try some math
    b.convolve(bin,a);
    
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << bin << std::endl;
  }


  void test_factor_speed() {
    const size_t max_dim = 16;
    typedef graphlab::table_factor<max_dim> factor_type;
    typedef factor_type::domain_type domain_type;
    typedef factor_type::assignment_type assignment_type;
    
    discrete_variable v1(1, 3);
    discrete_variable v2(2, 4);
    discrete_variable v3(3, 2);
    discrete_variable v4(4, 7);
    discrete_variable v5(5, 5);
    

    const domain_type big_dom = domain_type(v1) + v2 + v3 + v4 + v5;
    // Create a factor over the domain
    factor_type factor(big_dom);
    factor.uniform();
    const size_t max_iterations = 10000;
    timer time;


    time.start();
    for(size_t i = 0; i < max_iterations; ++i) {
      factor *= factor;
    }
    std::cout << "factor *= factor:    " 
              << time.current_time() << std::endl;


    const domain_type small_dom = domain_type(v1) + v2 + v3;
    factor_type small_factor(small_dom);
    small_factor.uniform();

    time.start();
    for(size_t i = 0; i < max_iterations; ++i) {
      factor *= small_factor;
    }
    std::cout << "factor *= small_factor:    " 
              << time.current_time() << std::endl;
    
 
  }

};
