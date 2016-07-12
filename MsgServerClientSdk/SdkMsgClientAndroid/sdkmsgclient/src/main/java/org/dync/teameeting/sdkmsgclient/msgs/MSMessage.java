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

    private int    millSec;//send second
    private int    flag;//group or single
    private int    tag;//txt or pic or self
    private int    role;//sender or recver

    public MSMessage() {
    }

    public MSMessage(String content, String extra, String msgId, String toId, String fromId, String groupId, int millSec, int flag, int tag, int role) {
        this.content = content;
        this.extra = extra;
        this.msgId = msgId;
        this.toId = toId;
        this.fromId = fromId;
        this.groupId = groupId;
        this.millSec = millSec;
        this.flag = flag;
        this.tag = tag;
        this.role = role;
    }

    public String getContent() {
        return content;
    }

    public String getExtra() {
        return extra;
    }

    public String getMsgId() {
        return msgId;
    }

    public String getToId() {
        return toId;
    }

    public String getFromId() {
        return fromId;
    }

    public String getGroupId() {
        return groupId;
    }

    public int getMillSec() {
        return millSec;
    }

    public int getFlag() {
        return flag;
    }

    public int getTag() {
        return tag;
    }

    public int getRole() {
        return role;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public void setExtra(String extra) {
        this.extra = extra;
    }

    public void setMsgId(String msgId) {
        this.msgId = msgId;
    }

    public void setToId(String toId) {
        this.toId = toId;
    }

    public void setFromId(String fromId) {
        this.fromId = fromId;
    }

    public void setGroupId(String groupId) {
        this.groupId = groupId;
    }

    public void setMillSec(int millSec) {
        this.millSec = millSec;
    }

    public void setFlag(int flag) {
        this.flag = flag;
    }

    public void setTag(int tag) {
        this.tag = tag;
    }

    public void setRole(int role) {
        this.role = role;
    }
}
