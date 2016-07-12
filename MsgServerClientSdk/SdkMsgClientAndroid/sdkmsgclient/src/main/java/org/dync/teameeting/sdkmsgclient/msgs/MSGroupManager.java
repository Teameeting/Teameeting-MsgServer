package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public class MSGroupManager {
    /**
     * add group delegate
     * <p/>
     * params:
     */
    public void addDelegate(MSGroupDelegate grpDelegate) {
        MsgClient.getInstance().MCSetGroupDelegate(grpDelegate);
    }

    /**
     * remove group delegate
     * <p/>
     * params:
     */
    public void delDelegate(MSGroupDelegate grpDelegate) {
        MsgClient.getInstance().MCSetGroupDelegate(null);
    }

    /**
     * add to group
     * <p/>
     * params:
     */
    public void addGroup(String grpId) {
        MsgClient.getInstance().MCAddGroup(grpId);
    }

    /**
     * remove from group
     * <p/>
     * params:
     */
    public void rmvGroup(String grpId) {
        MsgClient.getInstance().MCRmvGroup(grpId);
    }
}
