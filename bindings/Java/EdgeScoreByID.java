/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class EdgeScoreByID {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected EdgeScoreByID(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(EdgeScoreByID obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        LinkPredJavaJNI.delete_EdgeScoreByID(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setI(int value) {
    LinkPredJavaJNI.EdgeScoreByID_i_set(swigCPtr, this, value);
  }

  public int getI() {
    return LinkPredJavaJNI.EdgeScoreByID_i_get(swigCPtr, this);
  }

  public void setJ(int value) {
    LinkPredJavaJNI.EdgeScoreByID_j_set(swigCPtr, this, value);
  }

  public int getJ() {
    return LinkPredJavaJNI.EdgeScoreByID_j_get(swigCPtr, this);
  }

  public void setScore(double value) {
    LinkPredJavaJNI.EdgeScoreByID_score_set(swigCPtr, this, value);
  }

  public double getScore() {
    return LinkPredJavaJNI.EdgeScoreByID_score_get(swigCPtr, this);
  }

  public EdgeScoreByID() {
    this(LinkPredJavaJNI.new_EdgeScoreByID(), true);
  }

}
