package org.dync.teameeting.sdkmsgclient.msgs;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by hp on 7/13/16.
 */
public class MsMsgUtil {

    private static MSMessage JSONToMessage(JSONObject jobj) throws JSONException {
        MSMessage mMsg = new MSMessage();
        if (jobj.has("content"))
            mMsg.setContent(jobj.getString("content"));
        else
            mMsg.setContent("");
        if (jobj.has("extra"))
            mMsg.setExtra(jobj.getString("extra"));
        else
            mMsg.setExtra("");
        if (jobj.has("msgId"))
            mMsg.setMsgId(jobj.getString("msgId"));
        else
            mMsg.setMsgId("");
        if (jobj.has("toId"))
            mMsg.setToId(jobj.getString("toId"));
        else
            mMsg.setToId("");
        if (jobj.has("fromId"))
            mMsg.setFromId(jobj.getString("fromId"));
        else
            mMsg.setFromId("");
        if (jobj.has("groupId"))
            mMsg.setGroupId(jobj.getString("groupId"));
        else
            mMsg.setGroupId("");
        if (jobj.has("nickName"))
            mMsg.setNickName(jobj.getString("nickName"));
        else
            mMsg.setNickName("");
        if (jobj.has("uiconUrl"))
            mMsg.setUiconUrl(jobj.getString("uiconUrl"));
        else
            mMsg.setUiconUrl("");
        if (jobj.has("cash"))
            mMsg.setCash(jobj.getString("cash"));
        else
            mMsg.setCash("");
        if (jobj.has("wishcont"))
            mMsg.setWishcont(jobj.getString("wishcont"));
        else
            mMsg.setWishcont("");
        if (jobj.has("toNickName"))
            mMsg.setToNickName(jobj.getString("toNickName"));
        else
            mMsg.setToNickName("");
        if (jobj.has("millSec"))
            mMsg.setMillSec(jobj.getInt("millSec"));
        else
            mMsg.setMillSec(0);
        if (jobj.has("msgType"))
            mMsg.setMsgType(jobj.getInt("msgType"));
        else
            mMsg.setMsgType(0);
        if (jobj.has("flag"))
            mMsg.setFlag(jobj.getInt("flag"));
        else
            mMsg.setFlag(0);
        if (jobj.has("push"))
            mMsg.setPush(jobj.getInt("push"));
        else
            mMsg.setPush(0);
        return mMsg;
    }

    private static JSONObject MessageToJSON(MSMessage mobj) throws JSONException {
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
        jsonObj.put("toNickName", mobj.getToNickName());
        jsonObj.put("millSec", mobj.getMillSec());
        jsonObj.put("msgType", mobj.getMsgType());
        jsonObj.put("flag", mobj.getFlag());
        jsonObj.put("push", mobj.getPush());
        return jsonObj;
    }

/////////////////////////////////////////////////

    public static MSMessage Encode2JsonWithTxtMsg(MSSubMessage.MSTxtMessage txtMsg, int msgType) {
        MSMessage mMsg = new MSMessage();
        mMsg.setContent(txtMsg.getContent());
        mMsg.setGroupId(txtMsg.getGroupId());
        mMsg.setPush(txtMsg.getPush());
        mMsg.setToId(txtMsg.getToId());
        mMsg.setNickName(MsgClient.getInstance().getmStrNname());
        mMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        mMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        mMsg.setToNickName(txtMsg.getToNickName());
        mMsg.setMsgType(msgType);

        return mMsg;
    }

    public static MSMessage Encode2JsonWithLivMsg(MSSubMessage.MSLivMessage livMsg, int msgType) {
        MSMessage mMsg = new MSMessage();
        mMsg.setGroupId(livMsg.getGroupId());
        mMsg.setToId(livMsg.getToId());
        mMsg.setPush(livMsg.getPush());
        mMsg.setNickName(MsgClient.getInstance().getmStrNname());
        mMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        mMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        mMsg.setToNickName(livMsg.getToNickName());
        mMsg.setFlag(livMsg.getFlag());
        mMsg.setMsgType(msgType);

        return mMsg;
    }

    public static MSMessage Encode2JsonWithRenMsg(MSSubMessage.MSRenMessage renMsg, int msgType) {
        MSMessage mMsg = new MSMessage();
        mMsg.setGroupId(renMsg.getGroupId());
        mMsg.setToId(renMsg.getToId());
        mMsg.setPush(renMsg.getPush());
        mMsg.setNickName(MsgClient.getInstance().getmStrNname());
        mMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        mMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        mMsg.setCash(renMsg.getCash());
        mMsg.setWishcont(renMsg.getWishcont());
        mMsg.setToNickName(renMsg.getToNickName());
        mMsg.setMsgType(msgType);

        return mMsg;
    }

    public static MSMessage Encode2JsonWithBlkMsg(MSSubMessage.MSBlkMessage blkMsg, int msgType) {
        MSMessage mMsg = new MSMessage();
        mMsg.setGroupId(blkMsg.getGroupId());
        mMsg.setToId(blkMsg.getToId());
        mMsg.setPush(blkMsg.getPush());
        mMsg.setNickName(MsgClient.getInstance().getmStrNname());
        mMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        mMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        mMsg.setToNickName(blkMsg.getToNickName());
        mMsg.setFlag(blkMsg.getFlag());
        mMsg.setMsgType(msgType);

        return mMsg;
    }

    public static MSMessage Encode2JsonWithFbdMsg(MSSubMessage.MSFbdMessage fbdMsg, int msgType) {
        MSMessage mMsg = new MSMessage();
        mMsg.setGroupId(fbdMsg.getGroupId());
        mMsg.setToId(fbdMsg.getToId());
        mMsg.setPush(fbdMsg.getPush());
        mMsg.setNickName(MsgClient.getInstance().getmStrNname());
        mMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        mMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        mMsg.setToNickName(fbdMsg.getToNickName());
        mMsg.setFlag(fbdMsg.getFlag());
        mMsg.setMsgType(msgType);

        return mMsg;
    }

    public static MSMessage Encode2JsonWithMgrMsg(MSSubMessage.MSMgrMessage mgrMsg, int msgType) {
        MSMessage mMsg = new MSMessage();
        mMsg.setGroupId(mgrMsg.getGroupId());
        mMsg.setToId(mgrMsg.getToId());
        mMsg.setPush(mgrMsg.getPush());
        mMsg.setNickName(MsgClient.getInstance().getmStrNname());
        mMsg.setUiconUrl(MsgClient.getInstance().getmStrUicon());
        mMsg.setFromId(MsgClient.getInstance().getmStrUserId());
        mMsg.setToNickName(mgrMsg.getToNickName());
        mMsg.setFlag(mgrMsg.getFlag());
        mMsg.setMsgType(msgType);

        return mMsg;
    }

    public static String JsonToString(MSMessage msg) {
        JSONObject jobj = null;
        try {
            jobj = MsMsgUtil.MessageToJSON(msg);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        if (null != jobj) {
            return jobj.toString();
        } else {
            return "";
        }
    }

    public static MSMessage DecodeJsonToMessage(String jsonStr) {

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
        MSMessage mMsg = null;
        try {
            mMsg = MsMsgUtil.JSONToMessage(obj);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return mMsg;
    }
}
