// $Id: WeightedDiscretePDF.h,v 1.4 2006/07/07 05:54:31 zr Exp $

namespace dunbarPoisson
{

template <class T>
class WDPDF_Node
{
private:
	bool m_mark;

public:
	WDPDF_Node<T> *parent, *left, *right;
	T key;
	float weight, sumWeights;

public:
	WDPDF_Node(T key_, float weight_, WDPDF_Node<T> *parent_);
	~WDPDF_Node();

	WDPDF_Node<T> *sibling() { return this==parent->left?parent->right:parent->left; }

	void markRed() { m_mark = true; }
	void markBlack() { m_mark = false; }
	bool isRed() { return m_mark; }
	bool isBlack() { return !m_mark; }
	bool leftIsBlack() { return !left || left->isBlack(); }
	bool rightIsBlack() { return !right || right->isBlack(); }
	bool leftIsRed() { return !leftIsBlack(); }
	bool rightIsRed() { return !rightIsBlack(); }
	void setSum() { sumWeights = weight + (left?left->sumWeights:0) + (right?right->sumWeights:0); }
};

template <class T>
class WeightedDiscretePDF
{
private:
	WDPDF_Node<T> *m_root;

public:
	WeightedDiscretePDF();
	~WeightedDiscretePDF();

	void insert(T item, float weight);
	void update(T item, float newWeight);
	void remove(T item);
	bool inTree(T item);
	
		/* pick a tree element according to its
		 * weight. p should be in [0,1).
		 */
	T choose(float p);

private:
	WDPDF_Node<T> **lookup(T item, WDPDF_Node<T> **parent_out);
	void split(WDPDF_Node<T> *node);
	void rotate(WDPDF_Node<T> *node);
	void lengthen(WDPDF_Node<T> *node);
	void propogateSumsUp(WDPDF_Node<T> *n);
};

#include <stdexcept>

//

template <class T>
WDPDF_Node<T>::WDPDF_Node(T key_, float weight_, WDPDF_Node<T> *parent_)
{
	m_mark = false;

	key = key_;
	weight = weight_;
	sumWeights = 0;
	left = right = 0;
	parent = parent_;
}

template <class T>
WDPDF_Node<T>::~WDPDF_Node()
{
	if (left) delete left;
	if (right) delete right;
}

//

template <class T>
WeightedDiscretePDF<T>::WeightedDiscretePDF()
{
	m_root = 0;
}

template <class T>
WeightedDiscretePDF<T>::~WeightedDiscretePDF()
{
	if (m_root) delete m_root;
}

template <class T>
void WeightedDiscretePDF<T>::insert(T item, float weight)
{
	WDPDF_Node<T> *p=0, *n=m_root;

	while (n) {
		if (n->leftIsRed() && n->rightIsRed())
			split(n);

		p = n;
		if (n->key==item) {
			throw std::domain_error("insert: argument(item) already in tree");
		} else {
			n = (item<n->key)?n->left:n->right;
		}
	}

	n = new WDPDF_Node<T>(item, weight, p);

	if (!p) {
		m_root = n;
	} else {
		if (item<p->key) {
			p->left = n;
		} else {
			p->right = n;
		}

		split(n);
	}

	propogateSumsUp(n);
}

template <class T>
void WeightedDiscretePDF<T>::remove(T item)
{
	WDPDF_Node<T> **np = lookup(item, 0);
	WDPDF_Node<T> *child, *n = *np;

	if (!n) {
		throw std::domain_error("remove: argument(item) not in tree");
	} else {
		if (n->left) {
			WDPDF_Node<T> **leftMaxp = &n->left;

			while ((*leftMaxp)->right)
				leftMaxp = &(*leftMaxp)->right;

			n->key = (*leftMaxp)->key;
			n->weight = (*leftMaxp)->weight;

			np = leftMaxp;
			n = *np;
		}

			// node now has at most one child

		child = n->left?n->left:n->right;
		*np = child;

		if (child) {
			child->parent = n->parent;

			if (n->isBlack()) {
				lengthen(child);
			}
		}

		propogateSumsUp(n->parent);

		n->left = n->right = 0;
		delete n;
	}
}

template <class T>
void WeightedDiscretePDF<T>::update(T item, float weight)
{
	WDPDF_Node<T> *n = *lookup(item, 0);

	if (!n) {
		throw std::domain_error("update: argument(item) not in tree");
	} else {
		float delta = weight - n->weight;
		n->weight = weight;

		for (; n; n=n->parent) {
			n->sumWeights += delta;
		}
	}
}

template <class T>
T WeightedDiscretePDF<T>::choose(float p)
{
	if (p<0.0 || p>=1.0) {
		throw std::domain_error("choose: argument(p) outside valid range");
	} else if (!m_root) {
		throw std::logic_error("choose: choose() called on empty tree");
	} else {
		float w = m_root->sumWeights * p;
		WDPDF_Node<T> *n = m_root;

		while (1) {
			if (n->left) {
				if (w<n->left->sumWeights) {
					n = n->left;
					continue;
				} else {
					w -= n->left->sumWeights;
				}
			}
			if (w<n->weight || !n->right) {
				break; // !n->right condition shouldn't be necessary, just sanity check
			}
			w -= n->weight;
			n = n->right;
		}

		return n->key;
	}
}

template <class T>
bool WeightedDiscretePDF<T>::inTree(T item)
{
	WDPDF_Node<T> *n = *lookup(item, 0);

	return !!n;
}

//

template <class T>
WDPDF_Node<T> **WeightedDiscretePDF<T>::lookup(T item, WDPDF_Node<T> **parent_out)
{
	WDPDF_Node<T> *n, *p=0, **np=&m_root;

	while ((n = *np)) {
		if (n->key==item) {
			break;
		} else {
			p = n;
			if (item<n->key) {
				np = &n->left;
			} else {
				np = &n->right;
			}
		}
	}

	if (parent_out)
		*parent_out = p;
	return np;
}

template <class T>
void WeightedDiscretePDF<T>::split(WDPDF_Node<T> *n)
{
	if (n->left) n->left->markBlack();
	if (n->right) n->right->markBlack();

	if (n->parent) {
		WDPDF_Node<T> *p = n->parent;

		n->markRed();

		if (p->isRed()) {
			p->parent->markRed();

				// not same direction
			if (!(	(n==p->left && p==p->parent->left) || 
					(n==p->right && p==p->parent->right))) {
			  rotate(n);
			  p = n;
			}

			rotate(p);
			p->markBlack();
		}
	}
}

template <class T>
void WeightedDiscretePDF<T>::rotate(WDPDF_Node<T> *n)
{
	WDPDF_Node<T> *p=n->parent, *pp=p->parent;

	n->parent = pp;
	p->parent = n;

	if (n==p->left) {
		p->left = n->right;
		n->right = p;
		if (p->left) p->left->parent = p;
	} else {
		p->right = n->left;
		n->left = p;
		if (p->right) p->right->parent = p;
	}

	n->setSum();
	p->setSum();
	
	if (!pp) {
		m_root = n;
	} else {
		if (p==pp->left) {
			pp->left = n;
		} else {
			pp->right = n;
		}
	}
}

template <class T>
void WeightedDiscretePDF<T>::lengthen(WDPDF_Node<T> *n)
{
	if (n->isRed()) {
		n->markBlack();
	} else if (n->parent) {
		WDPDF_Node<T> *sibling = n->sibling();

		if (sibling && sibling->isRed()) {
			n->parent->markRed();
			sibling->markBlack();

			rotate(sibling); // node sibling is now old sibling child, must be black
			sibling = n->sibling();
		}

		// sibling is black

		if (!sibling) {
			lengthen(n->parent);
		} else if (sibling->leftIsBlack() && sibling->rightIsBlack()) {
			if (n->parent->isBlack()) {
				sibling->markRed();
				lengthen(n->parent);
			} else {
				sibling->markRed();
				n->parent->markBlack();
			}
		} else {
			if (n==n->parent->left && sibling->rightIsBlack()) {
				rotate(sibling->left); // sibling->left must be red
				sibling->markRed();
				sibling->parent->markBlack();
				sibling = sibling->parent;
			} else if (n==n->parent->right && sibling->leftIsBlack()) {
				rotate(sibling->right); // sibling->right must be red
				sibling->markRed();
				sibling->parent->markBlack();
				sibling = sibling->parent;
			}

			// sibling is black, and sibling's far child is red

			rotate(sibling);
			if (n->parent->isRed()) sibling->markRed();
			sibling->left->markBlack();
			sibling->right->markBlack();
		}
	}
}

template <class T>
void WeightedDiscretePDF<T>::propogateSumsUp(WDPDF_Node<T> *n)
{
	for (; n; n=n->parent)
		n->setSum();
}

}