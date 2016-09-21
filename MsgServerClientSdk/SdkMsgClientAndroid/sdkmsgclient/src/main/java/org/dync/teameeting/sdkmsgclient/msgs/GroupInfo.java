package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/11/16.
 */
public class GroupInfo {
    private String mUserId;
    private String mSeqnId;
    private long   mSeqn;
    private int    mIsFetched;


    public String getmUserId() { return mUserId; }

    public void setmUserId(String mUserId) { this.mUserId = mUserId; }

    public String getmSeqnId() {
        return mSeqnId;
    }

    public void setmSeqnId(String mSeqnId) {
        this.mSeqnId = mSeqnId;
    }

    public long getmSeqn() {
        return mSeqn;
    }

    public void setmSeqn(long mSeqn) {
        this.mSeqn = mSeqn;
    }

    public int getmIsFetched() {
        return mIsFetched;
    }

    public void setmIsFetched(int mIsFetched) {
        this.mIsFetched = mIsFetched;
    }

    @Override
    public String toString() {
        return "GroupInfo{" +
                "mUserId='" + mUserId + '\'' +
                "mSeqnId='" + mSeqnId + '\'' +
                ", mSeqn=" + mSeqn +
                ", mIsFetched=" + mIsFetched +
                '}';
    }
}
