import { Injectable } from '@angular/core';
import { CanDeactivate } from '@angular/router';
import { Observable } from 'rxjs';

export interface CanComponentDeactivate {
  canDeactivate: () => Observable<boolean> | Promise<boolean> | boolean;
}

@Injectable({
  providedIn: 'root'
})
export class CanDeactivateGuard implements CanDeactivate<CanComponentDeactivate> {
  canDeactivate(component: CanComponentDeactivate) {
    // console.log('en CanDeactive');
    return component.canDeactivate ? component.canDeactivate() : true;
  }
}

/* // Allow navigation if the component says that it is OK or it doesn't have a canDeactivate function
if (!component.canDeactivate || component.canDeactivate()) {
  return true;
} */
