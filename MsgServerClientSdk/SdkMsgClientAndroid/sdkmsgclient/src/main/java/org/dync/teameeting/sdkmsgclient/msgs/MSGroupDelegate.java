package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public interface MSGroupDelegate {
    /**
     * when add an group success
     * <p/>
     * params:
     */
    public void OnAddGroupSuccess(String grpId);

    /**
     * when add an group failed
     * <p/>
     * params:
     */
    public void OnAddGroupFailed(String grpId, String reason, int code);

    /**
     * when remove an group success
     * <p/>
     * params:
     */
    public void OnRmvGroupSuccess(String grpId);

    /**
     * when remove an group failed
     * <p/>
     * params:
     */
    public void OnRmvGroupFailed(String grpId, String reason, int code);
}
