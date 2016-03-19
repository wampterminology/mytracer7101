<?php

//
//Implement mytracer_config_form
//
function mytracer_config_form($form, &$form_state) {
 $form['mytracer_how_many'] = array(
  '#type' => 'textfield', 
  '#title' => t('设置追踪笔数'), 
  '#description' => t('设置页签下显示笔数为多少，设置数需要在5-20笔之间。'),
  '#default_value' => variable_get('mytracer_how_many', 0), 
  '#size' => 20, 
  '#maxlength' => 20, 
  '#required' => TRUE,
);
  return system_settings_form($form);
}
function mytracer_config_form_validate($form, &$form_state) {
  //dpm($form_state);
  $number=(int)$form_state['values']['mytracer_how_many'];
  if(($number<5)||($number>20)){
    form_set_error('mytracer_how_many', '數字需介於 5 到 20.');
  }
}




//
//Implement mytracer_records
//
function mytracer_records_table($account){
  global $user;
  //$rows=array();
  $query = db_select('mytracer', 'mt')->condition('mt.uid_was_visited', $account->uid, '='); 
      $query->fields('mt', array('uid_watched', 'visited_time', 'ip'));
      $query->join('users', 'u', 'u.uid = mt.uid_watched');
      $query->fields('u',array('name'))->orderBy('visited_time', 'DESC');
//      dpm((string) $query); 
//      $result = $query->execute(); 
//      dpm($result); 
  $header = array(
    array('data' => t('名字'),      'field' => 'uid_watched', ),
    array('data' => t('访问时间'),  'field' => 'visited_time', 'sort' => 'DESC',),
    array('data' => t('IP地址'),    'field' => 'ip', ),
    );
  $query=$query->extend('PagerDefault')->limit(10)->extend('TableSort')->orderByHeader($header);
  //$query=$query->extend('PagerDefault')->limit(20)->extend('TableSort')->orderBy('visited_time', 'DESC');
  $result=$query->execute();
  $rows=array();
  $i=1;
  foreach($result as $record){
    $number=  variable_get('mytracer_how_many', 0);
    if($i>$number){break;}
    $watched_obj=user_load($record->uid_watched);
    $username_link=theme('username',array('account'=>$watched_obj));
    $time=date('Y-m-d h:m:s',$record->visited_time);
    $rows[]=array($username_link,$time,$record->ip,);
    $i++;
  }
  
  $output['content']=array(
    '#theme'=>'table',
    '#header'=>$header,
    '#rows'=>$rows,
    '#caption' => '<h3>' . '最近這些人看過你的個人資訊' . '</h3>', 
    '#sticky' => true, 
    '#empty' => '目前沒有資料', 
    );
  $output['pager'] = array(
    '#theme' => 'pager', 
    '#weight' => 5, 
    );
  
  return $output;
}