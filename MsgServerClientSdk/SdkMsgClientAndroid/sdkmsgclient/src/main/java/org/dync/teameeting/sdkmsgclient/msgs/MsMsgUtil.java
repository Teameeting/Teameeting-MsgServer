package org.dync.teameeting.sdkmsgclient.msgs;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by hp on 7/13/16.
 */
public class MsMsgUtil {

    private static MSTxtMessage JSONToMessage(JSONObject jobj) throws JSONException {
        MSTxtMessage txtMsg = new MSTxtMessage();
        if (jobj.has("content"))
            txtMsg.setContent(jobj.getString("content"));
        else
            txtMsg.setContent("");
        if (jobj.has("extra"))
            txtMsg.setExtra(jobj.getString("extra"));
        else
            txtMsg.setExtra("");
        if (jobj.has("msgId"))
            txtMsg.setMsgId(jobj.getString("msgId"));
        else
            txtMsg.setMsgId("");
        if (jobj.has("toId"))
            txtMsg.setToId(jobj.getString("toId"));
        else
            txtMsg.setToId("");
        if (jobj.has("fromId"))
            txtMsg.setFromId(jobj.getString("fromId"));
        else
            txtMsg.setFromId("");
        if (jobj.has("groupId"))
            txtMsg.setGroupId(jobj.getString("groupId"));
        else
            txtMsg.setGroupId("");
        if (jobj.has("nickName"))
            txtMsg.setNickName(jobj.getString("nickName"));
        else
            txtMsg.setNickName("");
        if (jobj.has("uiconUrl"))
            txtMsg.setUiconUrl(jobj.getString("uiconUrl"));
        else
            txtMsg.setUiconUrl("");
        if (jobj.has("cash"))
            txtMsg.setCash(jobj.getString("cash"));
        else
            txtMsg.setCash("");
        if (jobj.has("wishcont"))
            txtMsg.setWishcont(jobj.getString("wishcont"));
        else
            txtMsg.setWishcont("");
        if (jobj.has("millSec"))
            txtMsg.setMillSec(jobj.getInt("millSec"));
        else
            txtMsg.setMillSec(0);
        if (jobj.has("msgType"))
            txtMsg.setMsgType(jobj.getInt("msgType"));
        else
            txtMsg.setMsgType(0);
        if (jobj.has("flag"))
            txtMsg.setFlag(jobj.getInt("flag"));
        else
            txtMsg.setFlag(0);
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
        jsonObj.put("cash", mobj.getCash());
        jsonObj.put("wishcont", mobj.getWishcont());
        jsonObj.put("millSec", mobj.getMillSec());
        jsonObj.put("msgType", mobj.getMsgType());
        jsonObj.put("flag", mobj.getFlag());
        return jsonObj;
    }

/////////////////////////////////////////////////

    public static String Encode2JsonWithGrpAndCont(String grpId, String cont, int msgType) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(cont);
        txtMsg.setGroupId(grpId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        txtMsg.setMsgType(msgType);

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

    public static String Encode2JsonWithUidAndCont(String userId, String cont, int msgType) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(cont);
        txtMsg.setToId(userId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        txtMsg.setMsgType(msgType);

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

    public static String Encode2JsonWithUidsAndCont(String[] userIds, String cont, int msgType) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setContent(cont);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        txtMsg.setMsgType(msgType);

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

    public static String Encode2JsonWithGrpAndUid(String grpId, String userId, int msgType) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setGroupId(grpId);
        txtMsg.setToId(userId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        txtMsg.setMsgType(msgType);

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

    public static String Encode2JsonWithGrpAndUid(String grpId, String userId, int flag, int msgType) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setGroupId(grpId);
        txtMsg.setToId(userId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        txtMsg.setFlag(flag);
        txtMsg.setMsgType(msgType);

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

    public static String Encode2JsonWithGrpAndUid(String grpId, String userId, String cash, String cont, int msgType) {
        MSTxtMessage txtMsg = new MSTxtMessage();
        txtMsg.setGroupId(grpId);
        txtMsg.setToId(userId);
        txtMsg.setNickName(MsgClient.getInstance().getmStrNname());
        txtMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        txtMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        txtMsg.setCash(cash);
        txtMsg.setWishcont(cont);
        txtMsg.setMsgType(msgType);

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
