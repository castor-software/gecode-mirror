/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Patrick Pekczynski <pekczynski@ps.uni-sb.de>
 *
 *  Copyright:
 *     Patrick Pekczynski, 2006
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace Gecode { namespace CpltSet {

  /**
   * \brief %Range iterator cache
   *
   * Allows to iterate the ranges as defined by the input iterator
   * several times provided the ValCache is %reset by the reset member
   * function.
   */

  template <class I>
  class ValCache : public Gecode::Iter::Values::IsValueIter<I> {
  protected:
    /// Array for ranges
    SharedArray<int> r;
    /// Current range
    int c;
    /// Number of ranges in cache
    int n;
    /// Number of elements in cache
    int s;
  public:
    /// \name Constructors and initialization
    //@{
    /// Default constructor
    ValCache(void);
    /// Initialize with ranges from \a i
    ValCache(I& i);
    /// Initialize with ranges from \a i
    void init(I& i);
    //@}

    /// \name Iteration control
    //@{
    /// Test whether iterator is still at a range or done
    bool operator ()(void) const;
    /// Move iterator to next range (if possible)
    void operator ++(void);
    /// Move iterator to previous range (if possible)
    void operator --(void);
    /// Reset iterator to start from beginning
    void reset(void);
    /// Start iteration from end
    void last(void);
    /// Stop iteration
    void finish(void);
    //@}

    /// \name %Range access
    //@{
    /// Always returns val
    int min(void) const;
    /// Always returns val
    int max(void) const;
    /// Return value
    int val(void) const;
    /// Return width of range (distance between minimum and maximum)
    unsigned int width(void) const;
    /// Return size of the union over all ranges
    unsigned int size(void) const;
    //@}

    /// \name %Index acces
    //@{
    /// Start iteration from ith range in cache
    void index(unsigned int i);
    /// Get the index of the current range
    unsigned int index(void);
    //@}

  };

  template <class I>
  forceinline
  ValCache<I>::ValCache(void) {}

  template <class I>
  inline void
  ValCache<I>::init(I& i) {
    Support::DynamicArray<int,Heap> d(heap);
    int j = 0;
    s = 0;
    while (i()) {
      d[j] = i.val();
      ++j; ++i;
      s++;
    }
    c = 0;
    n = j;

    r.init(n);
    for (int j = n; j--; )
      r[j] = d[j];
    c = 0;
  }

  template <class I>
  inline
  ValCache<I>::ValCache(I& i) {
    init(i);
  }

  template <class I>
  forceinline void
  ValCache<I>::operator ++(void) {
    c++;
  }

  template <class I>
  forceinline void
  ValCache<I>::operator --(void) {
    c--;
  }

  template <class I>
  forceinline bool
  ValCache<I>::operator ()(void) const {
    return -1 < c && c < n;
  }

  template <class I>
  forceinline void
  ValCache<I>::reset(void) {
    c = 0;
  }

  template <class I>
  forceinline void
  ValCache<I>::last(void) {
    c = n - 1;
  }

  template <class I>
  forceinline void
  ValCache<I>::finish(void) {
    c = -1;
  }

  template <class I>
  forceinline int
  ValCache<I>::min(void) const {
    return r[c];
  }

  template <class I>
  forceinline int
  ValCache<I>::max(void) const {
    return r[c];
  }

  template <class I>
  forceinline int
  ValCache<I>::val(void) const {
    return r[c];
  }

  template <class I>
  forceinline unsigned int
  ValCache<I>::width(void) const {
    return 1;
  }

  template <class I>
  forceinline unsigned int
  ValCache<I>::size(void) const {
    return s;
  }

  template <class I>
  forceinline void
  ValCache<I>::index(unsigned int i) {
    // maybe add an exception here
    c = i;
  }

  template <class I>
  forceinline unsigned int
  ValCache<I>::index(void) {
    return c;
  }

  /// Check whether range specifications for initialization are consistent
  forceinline void
  testConsistency(const IntSet& glb, const IntSet& lub,
                  const int cardMin, const int cardMax,
                  const char* location) {

    bool glbNonZero = glb.size() > 0;
    bool lubNonZero = lub.size() > 0;

    if (glbNonZero) {
      int  glbMin = glb.min();
      int  glbMax = glb.max();
      if (
          !lubNonZero ||
          (glbMin > glbMax)
          )
        throw CpltSet::VariableFailedDomain(location);

      if (
          (glbMin < Set::Limits::min) ||
          (glbMax > Set::Limits::max)
          )
        throw CpltSet::VariableOutOfRangeDomain(location);

      int  lubMin = lub.min();
      int  lubMax = lub.max();
      if (
          (glbMin < lubMin || glbMin > lubMax) ||
          (glbMax > lubMax || glbMax < lubMin)
          )
        throw CpltSet::VariableFailedDomain(location);
    }

    if (lubNonZero) {
      int  lubMin = lub.min();
      int  lubMax = lub.max();
      if (
          (lubMin < Set::Limits::min) ||
          (lubMax > Set::Limits::max)
         )
        throw CpltSet::VariableOutOfRangeDomain(location);

      if (lubMin > lubMax)
      throw CpltSet::VariableFailedDomain(location);
    }

    if (cardMax < 0)
      throw CpltSet::VariableFailedDomain(location);

    if ((unsigned int) cardMax > Set::Limits::card)
      throw CpltSet::VariableOutOfRangeCardinality(location);

    if (
        (cardMin > cardMax) ||
        (cardMin < 0)
        )
      throw CpltSet::VariableFailedDomain(location);
    return;
  }

  template <class View>
  void
  variableorder(Space& home, ViewArray<View>& x) {
    unsigned int var_in_tab = manager.allocated();

    unsigned int min_offset = x[0].offset();
    unsigned int max_width  = x[0].tableWidth();
    for (int i = 0; i < x.size(); i++) {
      if (x[i].offset() < min_offset) {
        min_offset = x[i].offset();
      }
      if (x[i].tableWidth() > max_width) {
        max_width = x[i].tableWidth();
      }
    }

    // constraint order as specified by Hawkins, Lagoon and Stuckey
    Region re(home);
    int* hls_order = re.alloc<int>(var_in_tab);

    // we do not care about variables
    // that are not in the scope of the constraint
    int c = 0;
    for (unsigned int i = 0; i < min_offset; i++, c++) {
      hls_order[i] = i;
    }

    // ordering those that lie in the scope
    for (unsigned int f = 0; f < max_width; f++) {
      for (int i = 0; i < x.size(); i++) {
        int xo = x[i].offset();
        int xw = x[i].tableWidth();
        int cur= xo + f;
        if (cur < xo + xw) {
          hls_order[c] = cur;
          c++;
        }
      }
    }

    // IMPORTANT DO NOT FORGET REMAINING LARGER VARIABLES
    for (unsigned int i = c; i < var_in_tab; i++, c++) {
      hls_order[i] = i;
    }

    manager.setorder(&hls_order[0]);
  }

  template <class View, class View1>
  void
  variableorder(Space& home, ViewArray<View>& x, ViewArray<View1>& y) {
    unsigned int var_in_tab = manager.allocated();

    unsigned int min_offset = x[0].offset();
    unsigned int max_width  = x[0].tableWidth();
    for (int i = 0; i < x.size(); i++) {
      if (x[i].offset() < min_offset) {
        min_offset = x[i].offset();
      }
      if (x[i].tableWidth() > max_width) {
        max_width = x[i].tableWidth();
      }
    }

    // constraint order as specified by Hawkins, Lagoon and Stuckey
    Region re(home);
    int* hls_order = re.alloc<int>(var_in_tab);

    // we do not care about variables
    // that are not in the scope of the constraint
    int c = 0;
    for (unsigned int i = 0; i < min_offset; i++, c++) {
      hls_order[i] = i;
    }

    // ordering those that lie in the scope
    for (unsigned int f = 0; f < max_width; f++) {
      for (int i = 0; i < x.size(); i++) {
        int xo = x[i].offset();
        int xw = x[i].tableWidth();
        int cur= xo + f;
        if (cur < xo + xw) {
          hls_order[c] = cur;
          c++;
        }
      }
      // INVARIANT: Here we assume that variables of the same array
      //            have the same initial values for min and max of the set
      //            bounds
      // invariant new arrays have subranges of old arrays
      for (int i = 0; i < y.size(); i++) {
        if ( (x[0].initialLubMin() + (int) f >= y[i].initialLubMin()) &&
             (x[0].initialLubMin() + (int) f <= y[i].initialLubMax())) {
          int xyshift = y[i].initialLubMin()  - x[0].initialLubMin();
          int yo = y[i].offset();
          int yw = y[i].tableWidth();
          int cur= yo + f - xyshift;
          if (cur <= yo + yw) {
            hls_order[c] = cur;
            c++;
          }
        }
      }
    }

    // Order remaining variables
    for (unsigned int i = c; i < var_in_tab; i++, c++) {
      hls_order[i] = i;
    }

    manager.setorder(&hls_order[0]);
  }

  /// Returns a bdd representing the convex hull of \a robdd
  GECODE_CPLTSET_EXPORT void conv_hull(bdd& robdd, bdd& hull);

  /// Returns a bdd representing all variables of \a domain
  GECODE_CPLTSET_EXPORT bdd bdd_vars(bdd& domain);


  /// Build the ROBDD for \f$ |x|=c\f$
  GECODE_CPLTSET_EXPORT bdd
  cardeq(Space& home, int offset, int c, int n, int r);

  /// Build the ROBDD for \f$ cl \leq |x| \leq cr\f$
  GECODE_CPLTSET_EXPORT bdd
  cardlqgq(Space& home, int offset, int cl, int cr, int n, int r);

  /// Build the ROBDD for \f$ cl \leq |x| \leq cr\f$
  GECODE_CPLTSET_EXPORT bdd
  cardcheck(Space& home, int xtab, int offset, int cl, int cr);

  // EXTENDED CARDINALITY FOR REPLACING INTERMEDIATE VARIABLES

  /// Build the ROBDD for \f$ |x|=c\f$
  template <class I, class View0, class View1>
  bdd
  extcardeq(Space& home, ValCache<I>& inter,
            View0& x, View1& y, unsigned int c, int n, int) {
    int xmin = x.initialLubMin();
    int ymin = y.initialLubMin();

    Region re(home);
    bdd* layer = re.alloc<bdd>(n);

    // build nodes for lowest layer
    layer[0] = bdd_true();
    inter.last();
    for (unsigned int i = 1; i <= c; i++) {
      int k = inter.min();
      layer[i] = x.element(k - xmin);
      layer[i] &= y.element(k - ymin);
    }

    // connect nodes in lowest layer
    for (int i = 1; i < n; i++) {
      layer[i] = manager.ite(layer[i], layer[i - 1], bdd_false());
    }

    inter.last();

    // build the remaining layers on top
    for (; inter(); --inter) {
      unsigned int pos = inter.index();
      for (int i = 0; i < n; i++) {
        int col = inter.min();
        bdd t = bdd_true();
        if (i == 0) {
          t = bdd_false();
        } else {
          t = layer[i-1];
        }
        bdd both = manager.ite(y.element(col - ymin), t,layer[i]);
        layer[i] = manager.ite(x.element(col - xmin), both ,layer[i]);
        --inter;
        if (!inter()) { break;}
      }
      if (!inter()) { break;}
      inter.index(pos);
    }
    return layer[n - 1];
  }


  /// Build the ROBDD for \f$ cl \leq |x| \leq cr\f$
  template <class I, class View0, class View1>
  bdd
  extcardlqgq(Space& home,
              ValCache<I>& inter, View0& x, View1& y,
              unsigned int cl, unsigned int cr, int n, int r) {
    Region re(home);
    bdd* layer = re.alloc<bdd>(n);

    // creates TOP v(c) v(c-1) ... v(c - cl + 1)
    layer[n - cl - 1] = bdd_true();

    inter.last();

    int k    = inter.min();
    int xmin = x.initialLubMin();
    int ymin = y.initialLubMin();

    // build nodes for lowest layer
    for (int i = n - cl ; i < n; i++, --inter) {
      k    = inter.min();
      bdd both = manager.ite(y.element(k - ymin), layer[i - 1], bdd_false());
      layer[i] = manager.ite(x.element(k - xmin), both, bdd_false());
    }

    // start with a shift and build layers up to the connection layer
    inter.last();
    --inter;

    // IMPORTANT we have to argue about the intersection cache like
    // an array with index acces k = kth range in the cache
    // hence we need an additional function that returns the index c
    // of the cache iterator

    for (; inter(); --inter) {
      unsigned int pos = inter.index(); // save position of k
      // cl < cr <= tab  ==> n - cl > 0
      for (int i = n - cl; i < n; i++) {
        int col = inter.min();
        bdd t = layer[i-1];
        bdd both = manager.ite(y.element(col - ymin), t, layer[i]);
        layer[i] = manager.ite(x.element(col - xmin), both, layer[i]);
        --inter;
        if ((int) inter.index() + 1 < r + 1 - (int) cr) {
          inter.finish(); break;
        }
      }
      if (!inter()) break;
      inter.index(pos);
    }

    if ((int) cr == r + 1) {
      // max card equals table width, all elements allowed
      return layer[n - 1];
    }

    if ((int) cr == r) {
      // only one single layer
      inter.last();
      int col  = inter.min();
      {
        bdd t = bdd_true();
        bdd f = bdd_true();
        bdd zerot = bdd_false();
        bdd zerof = t;
        for (int i = 0; i < n; i++) {
          col  = inter.min();
          if (i == 0) {
            t = zerot;
            f = zerof;
          } else {
            t = layer[i-1];
            if (i > n - (int) cl - 1) { // connect lower layer
              f = layer[i];
            }
          }
          bdd both = manager.ite(y.element(col - ymin), t ,f);
          layer[i] = manager.ite(x.element(col - xmin), both ,f);
          --inter;
          if (!inter()) { break;}
        }
      }
      return layer[n- 1];
    }

    inter.last();
    // connection layer between cl and cr
    {
      bdd t = bdd_true();
      bdd f = bdd_true();
      for (int i = 0; i < n; i++) {
        int col = inter.min();
        if (i == 0) {
          t = bdd_false();
        } else {
          t = layer[i-1];
          // NOTE: ONLY CONNECT if cl > 0
          if (i > n - (int) cl - 1 && cl > 0) { // connect lower layer
            f = layer[i];
          }
        }
        bdd both = manager.ite(y.element(col - ymin), t ,f);
        layer[i] = manager.ite(x.element(col - xmin), both ,f);
        --inter;
        if (!inter()) { break;}
      }
    }

    // the remaining layers for cr
    inter.last();
    --inter;
    for (; inter(); --inter) {
      unsigned int pos = inter.index();
      for (int i = 0; i < n; i++) {
        int col  = inter.min();
        bdd t = bdd_true();
        if (i == 0) {
          t = bdd_false();
        } else {
          t = layer[i-1];
        }
        // i guess here we do a little too much
        bdd both = y.element(col - ymin) & x.element(col - xmin);
        layer[i] = manager.ite(both, t, layer[i]);

        --inter;
        if (!inter()) {
          break;
        }
      }
      if (!inter()) {
        break;
      }
      inter.index(pos);
    }

    return layer[n - 1];
  }

  /// Build the ROBDD for \f$ cl \leq |x \cap y| \leq cr\f$
  template <class View0, class View1>
  bdd
  extcardcheck(Space& home,
               View0& x, View1& y, unsigned int cl, unsigned int cr) {
    // Ad 2)
    // we need manager and variable offset for the bddtable
    // because we do not want to rewrite the code for the simple checks
    // everytime

    // Ad 3)
    // the same extensions for the respective proper cardinality functions

    // Compute the intersection of x and y  and bring it into a data structure
    // where iteration may start with the greatest element of the intersection
    Set::LubRanges<View0> lubx(x);
    Set::LubRanges<View1> luby(y);
    // common values
    Gecode::Iter::Ranges::Inter<Set::LubRanges<View0>, Set::LubRanges<View1> >
      common(lubx, luby);
    // get it cached
    Gecode::Iter::Ranges::ToValues<
      Gecode::Iter::Ranges::Inter<Set::LubRanges<View0>,
        Set::LubRanges<View1> > > values(common);

    ValCache<
      Gecode::Iter::Ranges::ToValues<
        Gecode::Iter::Ranges::Inter<Set::LubRanges<View0>,
          Set::LubRanges<View1> > > > inter(values);

    // compute the size of the intersection
    unsigned int isize = inter.size();

    if (cr > isize) {
      cr = isize;
    }
    int r = isize - 1; // rightmost bit in bitvector
    int n = cr + 1; // layer size
    if (cl > isize || cl > cr) { // inconsistent cardinality
      return bdd_false();
    }

    if (cr == 0) {    // cl <= cr
      // build the emptyset
      bdd empty = bdd_true();
      for (; inter(); ++inter) {
        int v = inter.min();
        assert(v >= x.initialLubMin());
        assert(v <= x.initialLubMax());
        assert(v >= y.initialLubMin());
        assert(v <= y.initialLubMax());
        empty &= (x.elementNeg(v - x.initialLubMin()) &
                  y.elementNeg(v - y.initialLubMin()));
      }
      return empty;
    }

    if (cl == cr) {
      if (cr == isize) {
        // build the full set
        bdd full = bdd_true();
        for (; inter(); ++inter) {
          int v = inter.min();
          assert(v >= x.initialLubMin());
          assert(v <= x.initialLubMax());
          assert(v >= y.initialLubMin());
          assert(v <= y.initialLubMax());
          full &= (x.element(v - x.initialLubMin()) &
                   y.element(v - y.initialLubMin()));
        }
        return full;
      } else {
        return extcardeq(home, inter, x, y, cr, n, r);
      }
    }

    // cl < cr
    if (cr == isize) {
      if (cl == 0) {   // no cardinality restriction
        return bdd_true();
      }
    }
    return extcardlqgq(home, inter, x, y, cl, cr, n, r);
  }


  // extcard with const intset
  template <class I>
  bdd
  cardConst(Space& home, int, int xoff, int xmin, int cl, int cr, I& is) {
    // Invariant: We require that the IntSet provided is a subset of the
    // variable range
    Gecode::Iter::Ranges::ToValues<I> ir(is);
    ValCache<Gecode::Iter::Ranges::ToValues<I> > inter(ir);

    int r = inter.size() - 1;
    int n = cr + 1;

    Region re(home);
    bdd* layer = re.alloc<bdd>(n);

    // creates TOP v(c) v(c-1) ... v(c - cl + 1)
    layer[n - cl - 1] = bdd_true();

    inter.last();

    int k    = inter.min();

    // build nodes for lowest layer
    for (int i = n - cl ; i < n; i++, --inter) {
      k    = inter.min();
      layer[i] = manager.ite(manager.bddpos(xoff + k - xmin), layer[i - 1],
                             bdd_false());
    }

    // start with a shift and build layers up to the connection layer
    inter.last();
    --inter;

    for (; inter(); --inter) {
      // save position of k
      unsigned int pos = inter.index();

      // cl < cr <= tab  ==> n - cl > 0
      for (int i = n - cl; i < n; i++) {
        int col = inter.min();
        bdd t = layer[i-1];
        layer[i] = manager.ite(manager.bddpos(xoff + col - xmin), t,
                               layer[i]);
        --inter;
        if ((int) inter.index() + 1 < r + 1 - (int) cr) {
          inter.finish(); break;
        }
      }
      if (!inter()) break;
      inter.index(pos);
    }

    if ((int) cr == r + 1) {
      // max card equals table width, all elements allowed
      return layer[n - 1];
    }

    if ((int) cr == r) {
      // only one single layer
      inter.last();
      int col  = inter.min();
      {
        bdd t = bdd_true();
        bdd f = bdd_true();
        bdd zerot = bdd_false();
        bdd zerof = t;
        for (int i = 0; i < n; i++) {
          col  = inter.min();
          if (i == 0) {
            t = zerot;
            f = zerof;
          } else {
            t = layer[i-1];
            if (i > n - (int) cl - 1) { // connect lower layer
              f = layer[i];
            }
          }
          layer[i] = manager.ite(manager.bddpos(xoff + col - xmin), t ,f);
          --inter;
          if (!inter()) { break;}
        }
      }
      return layer[n- 1];
    }

    inter.last();
    // connection layer between cl and cr
    {
      bdd t = bdd_true();
      bdd f = bdd_true();
      for (int i = 0; i < n; i++) {
        int col = inter.min();
        if (i == 0) {
          t = bdd_false();
        } else {
          t = layer[i-1];
          // NOTE: ONLY CONNECT if cl > 0
          if (i > n - (int) cl - 1 && cl > 0) { // connect lower layer
            f = layer[i];
          }
        }
        layer[i] = manager.ite(manager.bddpos(xoff + col - xmin), t ,f);
        --inter;
        if (!inter()) { break;}
      }
    }

    // the remaining layers for cr
    inter.last();
    --inter;
    for (; inter(); --inter) {
      unsigned int pos = inter.index();
      for (int i = 0; i < n; i++) {
        int col  = inter.min();
        bdd t = bdd_true();
        if (i == 0) {
          t = bdd_false();
        } else {
          t = layer[i-1];
        }
        layer[i] = manager.ite(manager.bddpos(xoff + col - xmin), t,
                               layer[i]);
        --inter;
        if (!inter()) {
          break;
        }
      }
      if (!inter()) {
        break;
      }
      inter.index(pos);
    }

    return layer[n - 1];
  }

  // mark all nodes in the dqueue
  GECODE_CPLTSET_EXPORT void
  extcache_mark(SharedArray<bdd>& nodes,
                int n, int& l, int& r, int& markref);

  // unmark all nodes in the dqueue
  GECODE_CPLTSET_EXPORT void
  extcache_unmark(SharedArray<bdd>& nodes,
                  int n, int& l, int& r, int& markref);

  // iterate to the next level of nodes
  GECODE_CPLTSET_EXPORT void
  extcardbounds(int& markref, bdd& c, int& n, int& l, int& r,
                bool& singleton, int& _level,
                SharedArray<bdd>& nodes,
                int& curmin, int& curmax, Gecode::IntSet& out);


  GECODE_CPLTSET_EXPORT void
  getcardbounds(bdd& c, int& curmin, int& curmax);
  // END EXTRACT CARDINALITY

  GECODE_CPLTSET_EXPORT bdd
  lexlt(unsigned int& xoff, unsigned int& yoff,
        unsigned int& range, int n);

  GECODE_CPLTSET_EXPORT bdd
  lexlq(unsigned int& xoff, unsigned int& yoff,
        unsigned int& range, int n);

  GECODE_CPLTSET_EXPORT bdd
  lexltrev(unsigned int& xoff, unsigned int& yoff,
           unsigned int& range, int n);

  GECODE_CPLTSET_EXPORT bdd
  lexlqrev(unsigned int& xoff, unsigned int& yoff,
           unsigned int& range, int n);

  template <class View>
  void quantify(bdd& p, View& x) {
    bdd dom = x.dom();
    int s = x.offset();
    int w = s + x.tableWidth() - 1;
    manager.existquant(p, dom, s, w);
    // \todo check the update line from naryrec.hpp "dom = p" ()
  }

}}

// STATISTICS: cpltset-support
