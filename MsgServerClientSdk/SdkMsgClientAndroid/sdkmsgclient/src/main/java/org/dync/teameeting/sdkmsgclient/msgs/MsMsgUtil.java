package org.dync.teameeting.sdkmsgclient.msgs;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by hp on 7/13/16.
 */
public class MsMsgUtil {

    private static MSTxtMessage JSONToMessage(JSONObject jobj) throws JSONException {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(jobj.getString("content"));
        txtMsg.setExtra(jobj.getString("extra"));
        txtMsg.setMsgId(jobj.getString("msgId"));
        txtMsg.setToId(jobj.getString("toId"));
        txtMsg.setFromId(jobj.getString("fromId"));
        txtMsg.setGroupId(jobj.getString("groupId"));
        txtMsg.setNickName(jobj.getString("nickName"));
        txtMsg.setUiconUrl(jobj.getString("uiconUrl"));
        txtMsg.setMillSec(jobj.getInt("millSec"));
        txtMsg.setFlag(jobj.getInt("flag"));
        txtMsg.setTag(jobj.getInt("tag"));
        txtMsg.setRole(jobj.getInt("role"));
        return txtMsg;
    }

    private static JSONObject MessageToJSON(MSTxtMessage mobj) throws JSONException {
        JSONObject jsonObj = new JSONObject();
        jsonObj.put("content", mobj.getContent());
        jsonObj.put("extra", mobj.getExtra());
        jsonObj.put("msgId", mobj.getMsgId());
        jsonObj.put("toId", mobj.getToId());
        jsonObj.put("fromId", mobj.getFromId());
        jsonObj.put("groupId", mobj.getGroupId());
        jsonObj.put("nickName", mobj.getNickName());
        jsonObj.put("uiconUrl", mobj.getUiconUrl());
        jsonObj.put("millSec", mobj.getMillSec());
        jsonObj.put("flag", mobj.getFlag());
        jsonObj.put("tag", mobj.getTag());
        jsonObj.put("role", mobj.getRole());
        return jsonObj;
    }

/////////////////////////////////////////////////

    public static String Encode2JsonWithGrpAndCont(String grpId, String cont) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(cont);
        txtMsg.setGroupId(grpId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());

        JSONObject jobj = null;
        try {
            jobj = MsMsgUtil.MessageToJSON(txtMsg);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (null != jobj) {
            return jobj.toString();
        } else {
            return "";
        }
    }

    public static String Encode2JsonWithUidAndCont(String userId, String cont) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(cont);
        txtMsg.setToId(userId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());

        JSONObject jobj = null;
        try {
            jobj = MsMsgUtil.MessageToJSON(txtMsg);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (null != jobj) {
            return jobj.toString();
        } else {
            return "";
        }
    }

    public static String Encode2JsonWithUidsAndCont(String[] userIds, String cont) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(cont);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());

        JSONObject jobj = null;
        try {
            jobj = MsMsgUtil.MessageToJSON(txtMsg);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (null != jobj) {
            return jobj.toString();
        } else {
            return "";
        }
    }

    public static String Encode2JsonWithGrpAndUid(String grpId, String userId) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setGroupId(grpId);
        txtMsg.setToId(userId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());

        JSONObject jobj = null;
        try {
            jobj = MsMsgUtil.MessageToJSON(txtMsg);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (null != jobj) {
            return jobj.toString();
        } else {
            return "";
        }
    }

    public static MSTxtMessage DecodeJsonToMessage(String jsonStr) {

        JSONObject obj = null;
        try {
            obj = new JSONObject(jsonStr);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (null == obj) {
            System.err.println("MsMsgUtil DecodeJsonToMessage error, obj is null");
            return null;
        }
        MSTxtMessage txtMsg = null;
        try {
            txtMsg = MsMsgUtil.JSONToMessage(obj);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return txtMsg;
    }
}
