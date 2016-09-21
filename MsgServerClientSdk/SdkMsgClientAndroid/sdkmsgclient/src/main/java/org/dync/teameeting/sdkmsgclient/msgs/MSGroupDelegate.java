package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public interface MSGroupDelegate {
    /**
     *  when add an group success this function will be called
     *
     *  params:
     *      grpId: the groupid you just added
     */
    public void OnAddGroupSuccess(String grpId);

    /**
     *  when add an group failed this function will be called
     *
     *  params:
     *      grpId: the groupid you just add
     *      reason: the reason failed
     *      code: the code failed
     */
    public void OnAddGroupFailed(String grpId, String reason, int code);

    /**
     *  when remove an group success this function will be called
     *
     *  params:
     *      grpId: the groupid you just remove
     */
    public void OnRmvGroupSuccess(String grpId);

    /**
     *  when remove an group failed this function will be called
     *
     *  params:
     *      grpId: the groupid you just remove
     *      reason: the reason failed
     *      code: the code failed
     */
    public void OnRmvGroupFailed(String grpId, String reason, int code);
}
