import sl4a
droid = sl4a.Android()

def alert_dialog_pnn_response(alert_name, alert_messagge):
    droid.dialogCreateAlert(alert_name, alert_messagge)
    droid.dialogSetPositiveButtonText('Yes')
    droid.dialogSetNegativeButtonText('No')
    droid.dialogSetNeutralButtonText('Cancel')
    droid.dialogShow()
    response = droid.dialogGetResponse().result
    return response['which'] in ('positive', 'negative', 'neutral')

def alert_dialog_n_response(alert_name, alert_messagg):
    droid.dialogCreateAlert(alert_name, alert_messagg)
    droid.dialogSetPositiveButtonText('OK')
    droid.dialogShow()
    response = droid.dialogGetResponse().result
    return response['which'] == 'positive'

def alert_toast(message):
  result = droid.makeToast(message)
  return result.error is None

def notify(title, notification):
    result = droid.notify(title, notification)
    return result.error is None

