#ifndef _distOp_h
#define _distOp_h

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <string>
#include "math.h"
using namespace std;

class Node {
  private:
    double q;
    double r;
    double rmin;
    long int count;
    double qDiff;
    double rDiff;
    Node *next;
    
  public:
    Node(double a, double b, double c, double d) {
        q = a;
        rmin = b;
        r = c;
        qDiff = d;
    }
    
    //Getters
    double getQ() const {
        return q;
    }
    double getRmin() const {
        return rmin;
    }
    double getR() const {
    	return r;
    }
    long int getCount() const {
        return count;
    }
    double getQDiff() const {
        return qDiff;
    }
    double getRDiff() const {
        return rDiff;
    }
    
    //Setters
    void setQ(double value) {
        q = value;
    }
    void setRmin(double value) {
        rmin = value;
    }
    void setR(double value) {
    	r = value;
    }
    void setCount(int c) {
        count = c;
    }
    void setQDiff(double x) { //Todo
        qDiff = x;
    }
    void setRDiff(double Rmin, double R) { //Todo
        rDiff=0;
    }

};

class NodeArray {
  private:
    Node **array;
    float size;
    double alpha;
    double zf;
    double xb;
    double xd;
    
  public:
    //Array Constructor
    NodeArray(int size, double alpha, double zf, double xb, double xd) {
        this->size = size;
        this->alpha = alpha;
        this->zf = zf;
        this->xb = xb;
        this->xd = xd;
        array = new Node*[size];
        for (long int i=0; i<size; i++) {
            array[i] = NULL;
        }
    }
    
    void calc() {
        double q=0, R=0, Rmin=0, xC=0, yC=0, yInt=0;
        long int index=0;
        
        for (double q=0.20; q<=0.30; q+=0.01) {
            Rmin = -0.3297*pow(q,6) + 0.4896*pow(q,5) + 0.0707*pow(q,4) - 
            0.374*pow(q,3) + 0.2698*pow(q,2) - 0.9485*q + 1.6321;
            //cout << "Rmin = " << Rmin << endl;
            for (double i=1.00; i<=1.25; i+=0.01) {
                //cout << "Q = " << q << endl;
                //cout << "Multiplying R by " << i << endl;
                R = Rmin * i;
                //cout << "R = " << R << endl;
                if (array[index]==NULL) {
                    array[index] = new Node(q, Rmin, R, qDiff(q));
                    yInt = yIntersect(xd, LOverV(R)); //Enriching line intersects y-axis
                    xC = xCollision(q, xd, zf, yInt); //x-coord of collision
                    yC = yCollision(q, zf, xC); //y-coord of collision
                    //cout << "yInt = " << yInt << endl;
                    //cout << "x-collision = " << xC << ", y-collision = " << yC << endl;
                    array[index]->setCount(count(alpha, xb, xd, xC, yC, yInt));
                    index++;
                    //cout << endl;
                }
                else {
                    cout << "Error: Array full." <<endl;
                    cout << "Last index: " << index << endl;
                    break;
                }
            }
        }
    }

    double qDiff(double q) {
    	if ((0.5 - q) < 0) 
    		return (q - 0.5);
    	else
    		return (0.5 - q);
    }
    
    double LOverV(double R) {
        return (R/(R+1));
    }

    /*double feedLine(double q, double zf, double x) {
     	return (-(q/(1-q))*x + (zf/(1-q)));
    }*/
    
    /*double xIntersect(double yInt, double zf, double q, double xd) {
        return ((yInt + (zf/(1-q)))/(((xd-yInt)/xd) + (q/(1-q))));
    }*/
    
    //Where the Enriching Line intersects the y-axis
    double yIntersect(double xd, double LOverV) {
        return (xd*(1 - LOverV));
    }
    
    double enrichingLine(double xd, double yInt, double x, double y, bool mode) {
        double m = ((xd - yInt)/xd);
        //cout << "slope = " << m << endl;
        if (mode == 0) //return y-coordinate
            return (((xd - yInt)/xd) * x + yInt);
        else //return x-coordinate
            return ((xd * (y - yInt))/(xd - yInt));
    }
    
    double strippingLine(double xb, double yC, double xC, double x, double y, bool mode) {
        double m = stripSlope(xb, yC, xC);
        //cout << "slope = " << m << endl;
        if (mode == 0) //return y-coordinate
            return (m*x + xb - m*x);
        else //return x-coordinate
            return ((xb * (m - 1) + y)/m);
    }

    double stripSlope(double xb, double yC, double xC) {
    	return ((xb - yC)/(xb - xC));
    }
    
    //x-coordinate of intersection at feed line
    double xCollision(double q, double xd, double zf, double yInt) {
        return ((xd * (yInt * (q-1) + zf))/(xd + yInt * (q-1)));
    }
    
    //y-coordinate of intersection at feed line
    double yCollision(double q, double zf, double xC) {
        return (-(q/(1-q))*xC + (zf/(1-q)));
    }
    
    //Vapor-Liquid Equilibrium Curve
    double VLE(double x, double a) { //some x, alpha
        return ((a*x)/((a-1)*x+1)); //y-coordinate
    }
    
    long int count(double a, double xb, double xd, double xC, double yC, double yInt) {
        double x=xb, y=xb;
        long int count=0;
        //cout << "Begin stepping..." << endl;
        
        do {
            //Go up, hit VLE line
            y = VLE(x, a);
            //Go right, hit stripping line
            if (strippingLine(xb, yC, xC, x, y, 1) < xC) {
                x = strippingLine(xb, yC, xC, x, y, 1);
                count ++;
                //cout << "(x,y) = " << x << ", " << y << endl;
            }
            else {
                x = enrichingLine(xd, yInt, x, y, 1);
                count++;
                //cout << "(x,y) = " << x << ", " << y << endl;
                break;
            }
        } while (count < 20);
        
        if (count < 20) {
            //cout << "Passed feed line." << endl;
            do {
                //Go up, hit VLE line
                y = VLE(x, a);
                //Go right, hit enriching line
                x = enrichingLine(xd, yInt, x, y, 1);
                //cout << "(x,y) = " << x << ", " << y << endl;
                count++;
            } while (x < xd);
        }
        /*else
            cout << "Stepping not completed." << endl;
        cout << "Count = " << count << endl;*/
        return count;
    }

    void print() const {
    	long int points = 0;
        cout << "Printing..." << endl;
        for (long int i=0; i<size; i++) {
            if (array[i]!=NULL && array[i]->getCount()==20) {
                cout << "Q = " << array[i]->getQ() << ", ";
                cout << "Rmin = " << array[i]->getRmin() << ", ";
                cout << "R = " << array[i]->getR() << ", ";
                cout << "qDiff = " << array[i]->getQDiff() << ", ";
                cout << "Count = " << array[i]->getCount() << endl;
                points++;
            }
            else if (array[i]==NULL) {
                cout << "End of array. " << points << " data points printed." << endl;
                break;
            }
        }
        cout << endl;
    }
    
};

#endif
