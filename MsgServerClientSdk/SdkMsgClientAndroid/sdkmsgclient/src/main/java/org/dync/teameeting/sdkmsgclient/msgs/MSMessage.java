package org.dync.teameeting.sdkmsgclient.msgs;

/**
 * Created by hp on 7/10/16.
 */
public class MSMessage {
    //@property (nonatomic, strong) NSDate* time;
    //@property (nonatomic, strong) NSDate* date;
    private String content;
    private String extra;
    private String msgId;
    private String toId;
    private String fromId;
    private String groupId;
    private String nickName;
    private String uiconUrl;

    private int    millSec;//send second
    private int    flag;//group or single
    private int    tag;//txt or pic or self
    private int    role;//sender or recver

    public MSMessage() {
    }

    public String getContent() {
        return content;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public String getExtra() {
        return extra;
    }

    public void setExtra(String extra) {
        this.extra = extra;
    }

    public String getMsgId() {
        return msgId;
    }

    public void setMsgId(String msgId) {
        this.msgId = msgId;
    }

    public String getToId() {
        return toId;
    }

    public void setToId(String toId) {
        this.toId = toId;
    }

    public String getFromId() {
        return fromId;
    }

    public void setFromId(String fromId) {
        this.fromId = fromId;
    }

    public String getGroupId() {
        return groupId;
    }

    public void setGroupId(String groupId) {
        this.groupId = groupId;
    }

    public String getNickName() {
        return nickName;
    }

    public void setNickName(String nickName) {
        this.nickName = nickName;
    }

    public String getUiconUrl() {
        return uiconUrl;
    }

    public void setUiconUrl(String uiconUrl) {
        this.uiconUrl = uiconUrl;
    }

    public int getMillSec() {
        return millSec;
    }

    public void setMillSec(int millSec) {
        this.millSec = millSec;
    }

    public int getFlag() {
        return flag;
    }

    public void setFlag(int flag) {
        this.flag = flag;
    }

    public int getTag() {
        return tag;
    }

    public void setTag(int tag) {
        this.tag = tag;
    }

    public int getRole() {
        return role;
    }

    public void setRole(int role) {
        this.role = role;
    }
}
