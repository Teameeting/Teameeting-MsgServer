package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public class MSGroupManager {
    /**
     *  add group delegate
     *
     *  params:
     *      grpDelegate: the group delegate for callback to set
     *
     *  return:
     *      void
     */
    public void addDelegate(MSGroupDelegate grpDelegate) {
        MsgClient.getInstance().MCSetGroupDelegate(grpDelegate);
    }

    /**
     *  remove group delegate
     *
     *  params:
     *      grpDelegate: the group delegate to delete
     *
     *  return:
     *      void
     */
    public void delDelegate(MSGroupDelegate grpDelegate) {
        MsgClient.getInstance().MCSetGroupDelegate(null);
    }

    /**
     *  add to group
     *
     *  params:
     *      grpId: the group id to add
     *
     *  return:
     *      void
     */
    public void addGroup(String grpId) {
        if (grpId.length()==0) return;
        MsgClient.getInstance().MCAddGroup(grpId);
    }

    /**
     *  remove from group
     *
     *  params:
     *      grpId: the group id to remove
     *
     *  return:
     *      void
     */
    public void rmvGroup(String grpId) {
        if (grpId.length()==0) return;
        MsgClient.getInstance().MCRmvGroup(grpId);
    }
}
